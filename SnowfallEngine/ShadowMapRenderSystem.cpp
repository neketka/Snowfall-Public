#include "ShadowMapRenderSystem.h"
#include "LightComponent.h"
#include "TransformComponent.h"
#include <glm/gtx/transform.hpp>
#include "Snowfall.h"

ShadowMapRenderSystem::ShadowMapRenderSystem() : m_cubeShadowsCount(16), m_flatShadowsCount(16)
{
	EngineSettings settings = Snowfall::GetGameInstance().GetEngineSettings();
	m_directionalHighShadow = new TextureAsset("", TextureType::Texture2D, TextureInternalFormat::Depth24I, settings.ShadowMapResolution, settings.ShadowMapResolution, 1, 1);
	m_flatShadows = new TextureAsset("", TextureType::Texture2DArray, TextureInternalFormat::Depth24I, settings.ShadowMapResolution, settings.ShadowMapResolution, m_flatShadowsCount, 1);
	m_cubeShadows = new TextureAsset("", TextureType::TextureCubemapArray, TextureInternalFormat::Depth24I, settings.ShadowMapResolution, settings.ShadowMapResolution, m_cubeShadowsCount, 1);

	m_highShadowTarget = new RenderTargetAsset("", { m_directionalHighShadow }, { TextureLayerAttachment(0, 0, -1) });
	m_cubeShadowTarget = new RenderTargetAsset("", { m_cubeShadows }, { TextureLayerAttachment(0, 0, -1) });
	m_flatShadowTarget = new RenderTargetAsset("", { m_flatShadows }, { TextureLayerAttachment(0, 0, -1) });
}

ShadowMapRenderSystem::~ShadowMapRenderSystem()
{
	m_directionalHighShadow->Unload();
	m_flatShadows->Unload();
	m_cubeShadows->Unload();

	delete m_directionalHighShadow;
	delete m_flatShadows;
	delete m_cubeShadows;
}

void ShadowMapRenderSystem::InitializeSystem(Scene& scene)
{
	m_scene = &scene;
}

glm::mat3 makeRotationDir(glm::vec3 direction)
{
	glm::vec3 xaxis = glm::normalize(glm::cross(glm::vec3(0, 1, 0), direction));
	glm::vec3 yaxis = glm::normalize(glm::cross(direction, xaxis));
	return glm::mat3(xaxis, yaxis, direction);
}

void ShadowMapRenderSystem::Update(float deltaTime)
{
	EngineSettings settings = Snowfall::GetGameInstance().GetEngineSettings();
	int flatsUsed = 0;
	int cubesUsed = 0;
	bool directional = false;

	glm::mat4 directionalCloseMatrix;
	LayerMask dirMask;

	std::vector<glm::mat4> pointMatrices;
	std::vector<glm::mat4> spotMatrices;

	std::vector<LayerMask> pointMasks;
	std::vector<LayerMask> spotMasks;

	for (LightComponent *light : m_scene->GetComponentManager().GetComponents<LightComponent>())
	{
		if (!light->Shadowing)
			continue;
		TransformComponent *transform = light->Owner.GetComponent<TransformComponent>();
		glm::mat4 view;
		glm::mat4 projection;
		switch (light->Type)
		{
		case LightType::Directional:
			view = glm::mat4(glm::mat3(transform->ModelMatrix)) * glm::translate(-transform->GlobalPosition);
			projection = glm::ortho(-10.f, 10.f, -10.f, 10.f, 1.0f, 10.0f);
			directionalCloseMatrix = light->lightSpace = projection * view;
			dirMask = light->LayerMask;
			directional = true;
			/*
			spotMatrices.push_back(directionalMatrix);
			spotMatrices.push_back(directionalMatrix);
			light->middleIndex = ++flatsUsed;
			light->lowIndex = ++flatsUsed; //Use with CSM
			*/
			break;
		case LightType::Spot:
			projection = glm::perspective(light->OuterCutoff / 2.f, 1.f, 0.3f, light->Range + 1);
			spotMatrices.push_back(light->lightSpace = projection * view);
			spotMasks.push_back(light->LayerMask);
			light->highIndex = ++flatsUsed;
			break;
		case LightType::Point:
			projection = glm::perspective(90.f, 1.f, 0.3f, light->Range + 1);

			view = glm::mat4(glm::mat3(makeRotationDir(glm::vec3(1, 0, 0)))) * glm::translate(-transform->GlobalPosition);
			pointMatrices.push_back(light->lightSpace = projection * view);

			view = glm::mat4(glm::mat3(makeRotationDir(glm::vec3(-1, 0, 0)))) * glm::translate(-transform->GlobalPosition);
			pointMatrices.push_back(light->lightSpace = projection * view);

			view = glm::mat4(glm::mat3(makeRotationDir(glm::vec3(0, 1, 0)))) * glm::translate(-transform->GlobalPosition);
			pointMatrices.push_back(light->lightSpace = projection * view);

			view = glm::mat4(glm::mat3(makeRotationDir(glm::vec3(0, -1, 0)))) * glm::translate(-transform->GlobalPosition);
			pointMatrices.push_back(light->lightSpace = projection * view);

			view = glm::mat4(glm::mat3(makeRotationDir(glm::vec3(0, 0, 1)))) * glm::translate(-transform->GlobalPosition);
			pointMatrices.push_back(light->lightSpace = projection * view);

			view = glm::mat4(glm::mat3(makeRotationDir(glm::vec3(0, 0, -1)))) * glm::translate(-transform->GlobalPosition);
			pointMatrices.push_back(light->lightSpace = projection * view);

			pointMasks.push_back(light->LayerMask);
			light->highIndex = ++cubesUsed;
			break;
		}
	}

	if (flatsUsed > m_flatShadowsCount)
	{
		m_flatShadowsCount = static_cast<int>(std::exp2(std::ceil(std::log2(flatsUsed))));
		m_flatShadowTarget->Unload();
		m_flatShadows->ResizeDepth(m_flatShadowsCount);
		delete m_flatShadowTarget;
		m_flatShadowTarget = new RenderTargetAsset("", { m_flatShadows }, { TextureLayerAttachment(0, 0, -1) });
	}
	if (cubesUsed > m_cubeShadowsCount)
	{
		m_cubeShadowsCount = static_cast<int>(std::exp2(std::ceil(std::log2(m_cubeShadowsCount))));
		m_cubeShadowTarget->Unload();
		m_cubeShadows->ResizeDepth(m_cubeShadowsCount);
		delete m_cubeShadowTarget;
		m_cubeShadowTarget = new RenderTargetAsset("", { m_cubeShadows }, { TextureLayerAttachment(0, 0, -1) });
	}

	if (directional)
		DoCloseDirectionalPass(directionalCloseMatrix, dirMask, settings.CloseShadowMapResolution);
	if (spotMatrices.size() > 0)
		DoSpotPass(spotMatrices, spotMasks, settings.ShadowMapResolution);
	if (pointMatrices.size() > 0)
		DoPointPass(pointMatrices, pointMasks, settings.ShadowMapResolution);
}

std::string ShadowMapRenderSystem::GetName()
{
	return "ShadowMapRenderSystem";
}

std::vector<std::string> ShadowMapRenderSystem::GetSystemsBefore()
{
	return { "TransformSystem", "MeshRenderingSystem" };
}

std::vector<std::string> ShadowMapRenderSystem::GetSystemsAfter()
{
	return { "CameraSystem" };
}

bool ShadowMapRenderSystem::IsMainThread()
{
	return true;
}

void ShadowMapRenderSystem::DoCloseDirectionalPass(glm::mat4 matrix, LayerMask mask, int size)
{
	IQuad2D region(0, 0, size, size);
	Pipeline pipeline;
	pipeline.FragmentStage.DepthTest = true;
	pipeline.FragmentStage.DepthMask = true;
	pipeline.FragmentStage.Framebuffer = m_highShadowTarget->GetFramebuffer();
	pipeline.FragmentStage.Viewport = region;
	pipeline.FragmentStage.DrawTargets = { };
	pipeline.VertexStage.FrontFaceCulling = false;

	ShaderConstants constants;
	constants.AddConstant(5, matrix);
	constants.AddConstant(11, 0);

	ShaderDescriptor desc;

	CommandBuffer buffer;
	m_highShadowTarget->GetFramebuffer().ClearDepth(1);
	Snowfall::GetGameInstance().GetMeshManager().Render(buffer, pipeline, constants, desc, mask, { "SHADOWPASS" }, false);
	buffer.ExecuteCommands();
}

void ShadowMapRenderSystem::DoPointPass(std::vector<glm::mat4> matrices, std::vector<LayerMask> masks, int size)
{
	IQuad2D region(0, 0, size, size);
	Pipeline pipeline;
	pipeline.FragmentStage.DepthTest = true;
	pipeline.FragmentStage.DepthMask = true;
	pipeline.FragmentStage.Framebuffer = m_cubeShadowTarget->GetFramebuffer();
	pipeline.FragmentStage.Viewport = region;
	pipeline.FragmentStage.DrawTargets = { };
	pipeline.VertexStage.FrontFaceCulling = true;

	ShaderDescriptor desc;

	CommandBuffer buffer;
	m_cubeShadowTarget->GetFramebuffer().ClearDepth(1);
	for (int i = 0; i < masks.size(); ++i)
	{
		ShaderConstants constants;
		constants.AddConstant(5, matrices[i * 6]);
		constants.AddConstant(6, matrices[i * 6 + 1]);
		constants.AddConstant(7, matrices[i * 6 + 2]);
		constants.AddConstant(8, matrices[i * 6 + 3]);
		constants.AddConstant(9, matrices[i * 6 + 4]);
		constants.AddConstant(10, matrices[i * 6 + 5]);
		constants.AddConstant(11, i);

		Snowfall::GetGameInstance().GetMeshManager().Render(buffer, pipeline, constants, desc, masks[i], { "SHADOWPASS", "CUBEPASS" }, false);
	}

	buffer.ExecuteCommands();
}

void ShadowMapRenderSystem::DoSpotPass(std::vector<glm::mat4> matrices, std::vector<LayerMask> masks, int size)
{
	IQuad2D region(0, 0, size, size);
	Pipeline pipeline;
	pipeline.FragmentStage.DepthTest = true;
	pipeline.FragmentStage.DepthMask = true;
	pipeline.FragmentStage.Framebuffer = m_flatShadowTarget->GetFramebuffer();
	pipeline.FragmentStage.Viewport = region;
	pipeline.FragmentStage.DrawTargets = { };
	pipeline.VertexStage.FrontFaceCulling = true;

	ShaderDescriptor desc;

	CommandBuffer buffer;
	m_flatShadowTarget->GetFramebuffer().ClearDepth(1);
	buffer.ClearDepthFramebufferCommand(-1);
	for (int i = 0; i < masks.size(); ++i)
	{
		ShaderConstants constants;
		constants.AddConstant(5, matrices[i]);
		constants.AddConstant(11, i);

		Snowfall::GetGameInstance().GetMeshManager().Render(buffer, pipeline, constants, desc, masks[i], { "SHADOWPASS" }, false);
	}

	buffer.ExecuteCommands();
}
