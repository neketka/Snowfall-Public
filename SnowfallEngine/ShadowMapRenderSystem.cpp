#include "stdafx.h"

#include "ShadowMapRenderSystem.h"
#include "LightComponent.h"
#include "TransformComponent.h"
#include <glm/gtx/transform.hpp>

ShadowMapRenderSystem::ShadowMapRenderSystem() : m_cubeShadowsCount(16), m_flatShadowsCount(16)
{
	EngineSettings settings = Snowfall::GetGameInstance().GetEngineSettings();
	m_flatShadows = new TextureAsset("", TextureType::Texture2DArray, TextureInternalFormat::Depth24I, settings.ShadowMapResolution, settings.ShadowMapResolution, m_flatShadowsCount, 1);
	m_cubeShadows = new TextureAsset("", TextureType::TextureCubemapArray, TextureInternalFormat::Depth24I, settings.ShadowMapResolution / 4, settings.ShadowMapResolution / 4, m_cubeShadowsCount, 1);

	m_highShadowTarget = new RenderTargetAsset("", { m_directionalHighShadow }, {}, { TextureLayerAttachment(0, 0, -1) });
	m_cubeShadowTarget = new RenderTargetAsset("", { m_cubeShadows }, {}, { TextureLayerAttachment(0, 0, -1) });
	m_flatShadowTarget = new RenderTargetAsset("", { m_flatShadows }, {}, { TextureLayerAttachment(0, 0, -1) });
}

ShadowMapRenderSystem::~ShadowMapRenderSystem()
{
	m_flatShadows->Unload();
	m_cubeShadows->Unload();

	m_highShadowTarget->Unload();
	m_cubeShadowTarget->Unload();
	m_flatShadowTarget->Unload();

	delete m_flatShadows;
	delete m_cubeShadows;

	delete m_highShadowTarget;
	delete m_cubeShadowTarget;
	delete m_flatShadowTarget;
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

void CalculateCascadeBoxes(std::vector<BoundingBox>& boxes, std::vector<float> splits, CameraComponent *camera, glm::mat4 lightView)
{
	std::vector<float> cascadeEnd;
	cascadeEnd.reserve(splits.size() + 2);
	cascadeEnd.push_back(-camera->ZNear);
	for (float split : splits)
		cascadeEnd.push_back(-split);
	
	float ar = static_cast<float>(camera->Region.Size.y) / camera->Region.Size.x;
	glm::mat4 camInv = glm::inverse(camera->ViewMatrix);

	float tanHalfHFov = glm::tan(glm::radians(camera->FovY / 2.f));
	float tanHalfVFov = glm::tan(glm::radians(camera->FovY * ar / 2.f));

	for (int i = 0; i < boxes.size(); ++i)
	{
		float xn = cascadeEnd[i] * tanHalfHFov;
		float xf = cascadeEnd[i + 1] * tanHalfHFov;
		float yn = cascadeEnd[i] * tanHalfVFov;
		float yf = cascadeEnd[i + 1] * tanHalfVFov;

		glm::vec4 frustumCorners[] = {
			glm::vec4(xn, yn, cascadeEnd[i], 1.f),
			glm::vec4(-xn, yn, cascadeEnd[i], 1.f),
			glm::vec4(xn, -yn, cascadeEnd[i], 1.f),
			glm::vec4(-xn, -yn, cascadeEnd[i], 1.f),

			glm::vec4(xf, yf, cascadeEnd[i + 1], 1.f),
			glm::vec4(-xf, yf, cascadeEnd[i + 1], 1.f),
			glm::vec4(xf, -yf, cascadeEnd[i + 1], 1.f),
			glm::vec4(-xf, -yf, cascadeEnd[i + 1], 1.f)
		};

		BoundingBox& box = boxes[i];
		box.MinExtent = glm::vec3(std::numeric_limits<float>::max());
		box.MaxExtent = glm::vec3(std::numeric_limits<float>::min());

		for (int j = 0; j < 8; ++j)
		{
			glm::vec3 vW = glm::vec3(lightView * camInv * frustumCorners[j]);
			box.MinExtent = glm::min(box.MinExtent, vW);
			box.MaxExtent = glm::max(box.MaxExtent, vW);
		}

		std::swap(box.MinExtent.z, box.MaxExtent.z);
		box.MinExtent.z = -box.MinExtent.z;
		box.MaxExtent.z = -box.MaxExtent.z;
	}

}

void ShadowMapRenderSystem::Update(float deltaTime)
{
	EngineSettings settings = Snowfall::GetGameInstance().GetEngineSettings();
	int flatsUsed = 0;
	int cubesUsed = 0;

	glm::mat4 biasMatrix(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
	);

	std::vector<glm::mat4> pointMatrices;
	std::vector<glm::mat4> spotMatrices;

	std::vector<LayerMask> pointMasks;
	std::vector<LayerMask> spotMasks;

	float farDirFactor = 1.f / settings.ShadowMapResolution;

	for (DirectionalLightComponent *light : m_scene->GetComponentManager().GetComponents<DirectionalLightComponent>())
	{
		if (!light->Enabled)
			continue;

		CameraComponent *cam = light->CascadeCamera.IsNull() ? m_scene->GetComponentManager().GetComponents<CameraComponent>()[0] 
			: light->CascadeCamera.GetComponent<CameraComponent>();

		if (!cam)
			cam = m_scene->GetComponentManager().GetComponents<CameraComponent>()[0];

		TransformComponent *transform = light->Owner.GetComponent<TransformComponent>();

		std::vector<BoundingBox> boxes(LIGHT_CASCADES);
		std::vector<float> distances(LIGHT_CASCADES);

		glm::mat4 view = glm::mat4(glm::mat3(transform->ModelMatrix)) * glm::translate(-transform->GlobalPosition);

		float dist = cam->ZNear;
		for (int i = 0; i < LIGHT_CASCADES; ++i)
		{
			dist += light->CascadeSizes[i];
			distances[i] = dist;
		}

		CalculateCascadeBoxes(boxes, distances, cam, view);

		int index = 0;
		for (BoundingBox& box : boxes)
		{
			box.MinExtent = glm::floor(box.MinExtent / farDirFactor) * farDirFactor;
			box.MaxExtent = glm::floor(box.MaxExtent / farDirFactor) * farDirFactor;

			glm::mat4 projection = glm::ortho(box.MinExtent.x, box.MaxExtent.x, box.MinExtent.y,
				box.MaxExtent.y, box.MinExtent.z, box.MaxExtent.z);;

			light->indices[index] = flatsUsed++;
			spotMatrices.push_back(light->lightSpaceMatrices[index] = projection * view);
			light->lightSpaceMatrices[index] = biasMatrix * light->lightSpaceMatrices[index];
			spotMasks.push_back(light->ShadowLayerMask);
			light->lightSpaceDistances[index] = distances[index];

			++index;
		}
	}
	
	for (SpotLightComponent *light : m_scene->GetComponentManager().GetComponents<SpotLightComponent>())
	{
		if (!light->Enabled)
			continue;
		TransformComponent *transform = light->Owner.GetComponent<TransformComponent>();

		glm::mat4 view = glm::mat4(glm::mat3(transform->ModelMatrix));
		glm::mat4 projection = glm::perspective(glm::radians(light->OuterCutoff * 2), 1.f, 1.f, light->Range + 1);

		spotMatrices.push_back(light->lightSpaceMatrix = projection * view);
		spotMasks.push_back(light->ShadowLayerMask);
		light->index = flatsUsed++;

		light->lightSpaceMatrix = biasMatrix * light->lightSpaceMatrix;
	}
	/*
			projection = glm::perspective(90.f, 1.f, 1.f, light->Range + 1);

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

			pointMasks.push_back(light->ShadowLayerMask);
			light->highIndex = flatsUsed++;*/

	if (flatsUsed > m_flatShadowsCount)
	{
		m_flatShadowsCount = static_cast<int>(std::exp2(std::ceil(std::log2(flatsUsed))));
		m_flatShadowTarget->Unload();
		m_flatShadows->ResizeDepth(m_flatShadowsCount);
		delete m_flatShadowTarget;
		m_flatShadowTarget = new RenderTargetAsset("", { m_flatShadows }, {}, { TextureLayerAttachment(0, 0, -1) });
	}
	if (cubesUsed > m_cubeShadowsCount)
	{
		m_cubeShadowsCount = static_cast<int>(std::exp2(std::ceil(std::log2(m_cubeShadowsCount))));
		m_cubeShadowTarget->Unload();
		m_cubeShadows->ResizeDepth(m_cubeShadowsCount);
		delete m_cubeShadowTarget;
		m_cubeShadowTarget = new RenderTargetAsset("", { m_cubeShadows }, {}, { TextureLayerAttachment(0, 0, -1) });
	}

	if (spotMatrices.size() > 0)
		DoSpotPass(spotMatrices, spotMasks, settings.ShadowMapResolution);
	if (pointMatrices.size() > 0)
		DoPointPass(pointMatrices, pointMasks, settings.ShadowMapResolution / 4);
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

void ShadowMapRenderSystem::DoPointPass(std::vector<glm::mat4> matrices, std::vector<LayerMask> masks, int size)
{
	Snowfall::GetGameInstance().GetMeshManager().RunCullingPass({});
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
		Snowfall::GetGameInstance().GetMeshManager().RunCullingPass({});
		ShaderConstants constants;
		constants.AddConstant(5, matrices[i * 6]);
		constants.AddConstant(6, matrices[i * 6 + 1]);
		constants.AddConstant(7, matrices[i * 6 + 2]);
		constants.AddConstant(8, matrices[i * 6 + 3]);
		constants.AddConstant(9, matrices[i * 6 + 4]);
		constants.AddConstant(10, matrices[i * 6 + 5]);
		constants.AddConstant(11, i);

		Snowfall::GetGameInstance().GetMeshManager().Render(buffer, pipeline, constants, desc, masks[i], { "SHADOWPASS", "CUBEPASS" }, false, false);
	}

	buffer.ExecuteCommands();
}

void ShadowMapRenderSystem::DoSpotPass(std::vector<glm::mat4> matrices, std::vector<LayerMask> masks, int size)
{
	Snowfall::GetGameInstance().GetMeshManager().RunCullingPass({});
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
	for (int i = 0; i < masks.size(); ++i)
	{
		Snowfall::GetGameInstance().GetMeshManager().RunCullingPass({});
		ShaderConstants constants;
		constants.AddConstant(5, matrices[i]);
		constants.AddConstant(11, i);

		Snowfall::GetGameInstance().GetMeshManager().Render(buffer, pipeline, constants, desc, masks[i], { "SHADOWPASS" }, false, false);
	}

	buffer.ExecuteCommands();
}
