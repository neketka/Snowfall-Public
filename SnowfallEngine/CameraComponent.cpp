#include "CameraComponent.h"
#include "TransformComponent.h"
#include "CommandBuffer.h"
#include "Snowfall.h"
#include "LightComponent.h"
#include "ShadowMapRenderSystem.h"
#include "SkyboxComponent.h"
#include "Scene.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

std::vector<SerializationField> ComponentDescriptor<CameraComponent>::GetSerializationFields()
{
	return {
		SerializationField("Region", SerializationType::ByValue, sizeof(IQuad2D), InterpretValueAs::IVector2Array),
		SerializationField("LayerMask", SerializationType::ByValue, sizeof(LayerMask), InterpretValueAs::UInt64),
		SerializationField("ZNear", SerializationType::ByValue, sizeof(float), InterpretValueAs::Float32),
		SerializationField("ZFar", SerializationType::ByValue, sizeof(float), InterpretValueAs::Float32),
		SerializationField("FovY", SerializationType::ByValue, sizeof(float), InterpretValueAs::Float32)
	};
}

CameraSystem::CameraSystem()
{
	m_quad = new MeshAsset("", Mesh({
		RenderVertex(glm::vec3(-1, -1, 1)),
		RenderVertex(glm::vec3(1, -1, 1)),
		RenderVertex(glm::vec3(1, 1, 1)),
		RenderVertex(glm::vec3(-1, 1, 1)),
	}, {
		0, 1, 2, 2, 3, 0
	}));
	for (int i = 0; i < 3; ++i)
	{
		m_shadowSamplers[i].SetWrapMode(TextureChannel::S, WrapMode::ClampToBorder);
		m_shadowSamplers[i].SetWrapMode(TextureChannel::T, WrapMode::ClampToBorder);
		m_shadowSamplers[i].SetWrapMode(TextureChannel::R, WrapMode::ClampToBorder);
		m_shadowSamplers[i].SetMinificationFilter(MinificationFilter::Linear);
		m_shadowSamplers[i].SetMagnificationFilter(MagnificationFilter::Linear);
		m_shadowSamplers[i].SetBorderColor(glm::vec4(1, 1, 1, 1));
		m_shadowSamplers[i].SetComparison(ComparisonFunc::LessEqual);
		m_shadowSamplers[i].SetCompareMode(true);
	}
}

CameraSystem::~CameraSystem()
{
	m_quad->Unload();
	m_cubeSampler.Destroy();
	for (int i = 0; i < 3; ++i)
	{
		m_shadowSamplers[i].Destroy();
	}
}

void CameraSystem::InitializeSystem(Scene& scene)
{
	m_scene = &scene;
}

void CameraSystem::Update(float deltaTime)
{
	TBuffer lights = dynamic_cast<LightSystem *>(m_scene->GetSystemManager().GetSystem("LightSystem"))->GetLightBuffer();
	ShadowMapRenderSystem *shadowRenderer = static_cast<ShadowMapRenderSystem *>(m_scene->GetSystemManager().GetSystem("ShadowMapRenderSystem"));
	CommandBuffer buffer;
	for (CameraComponent *camera : m_scene->GetComponentManager().GetComponents<CameraComponent>())
	{
		camera->colorAttachment = 0;
		if (camera->ViewportIndex != -1)
			camera->Region = Snowfall::GetGameInstance().GetViewport(camera->ViewportIndex);

		if (camera->oldRegion != camera->Region)
		{
			if (camera->HdrBuffer)
			{
				camera->HdrBuffer->Unload();
				delete camera->HdrBuffer;
			}

			camera->HdrBuffer = new RenderTargetAsset("", 
			{
					new TextureAsset("", TextureType::Texture2D,
				TextureInternalFormat::RGBA32F, camera->Region.Size.x, camera->Region.Size.y, 1, 1),
					new TextureAsset("", TextureType::Texture2D, 
				TextureInternalFormat::RGBA32F, camera->Region.Size.x, camera->Region.Size.y, 1, 1),
					new TextureAsset("", TextureType::Texture2D,
				TextureInternalFormat::Depth24I, camera->Region.Size.x, camera->Region.Size.y, 1, 1)
			}, 
			{ TextureLayerAttachment(0, 0, -1), TextureLayerAttachment(1, 0, -1), TextureLayerAttachment(2, 0, -1) });

			camera->oldRegion = camera->Region;
		}

		camera->colorAttachment = 0;

		Snowfall::GetGameInstance().GetMeshManager().RunCullingPass({});
		TransformComponent *transform = camera->Owner.GetComponent<TransformComponent>();

		camera->HdrBuffer->GetFramebuffer().ClearColor(0, glm::vec4(0, 0, 0, 1));
		camera->HdrBuffer->GetFramebuffer().ClearDepth(1);

		camera->ProjectionMatrix = glm::perspective(glm::radians(camera->FovY), static_cast<float>(camera->Region.Size.x) / static_cast<float>(camera->Region.Size.y), camera->ZNear, camera->ZFar);
		camera->ViewMatrix = glm::mat4(glm::mat3(transform->ModelMatrix)) * glm::translate(-transform->GlobalPosition);

		auto skyboxes = m_scene->GetComponentManager().GetComponents<SkyboxComponent>();
		if (skyboxes.size() > 0)
			RenderSkybox(buffer, camera, glm::inverse(camera->ProjectionMatrix * camera->ViewMatrix), skyboxes[0]->Cubemap);

		std::vector<int> indices;
		int index = 0;
		for (LightComponent *light : m_scene->GetComponentManager().GetComponents<LightComponent>())
		{
			if (index == 31)
				break;
			indices.push_back(index);
			++index;
		}

		lights.CopyData(&index, 0, 4);
		lights.CopyData(indices.data(), 4, 4 * index);

		Pipeline pipeline;
		pipeline.FragmentStage.DepthTest = true;
		pipeline.FragmentStage.DepthMask = true;
		pipeline.FragmentStage.Framebuffer = camera->HdrBuffer->GetFramebuffer();
		pipeline.FragmentStage.Viewport = camera->Region.NoOffset();
		pipeline.FragmentStage.DrawTargets = { 0 };
		pipeline.VertexStage.BackFaceCulling = true;

		ShaderConstants constants;
		constants.AddConstant(0, camera->ProjectionMatrix);
		constants.AddConstant(1, camera->ViewMatrix);
		constants.AddConstant(4, transform->Position);
		constants.AddConstant(12, shadowRenderer->GetDirectionHighShadowTexture()->GetTextureObject(), m_shadowSamplers[0]);
		constants.AddConstant(13, shadowRenderer->GetFlatShadowTexture()->GetTextureObject(), m_shadowSamplers[1]);
		constants.AddConstant(14, shadowRenderer->GetCubeShadowTexture()->GetTextureObject(), m_shadowSamplers[2]);

		ShaderDescriptor desc;
		desc.AddShaderStorageBuffer(lights, 2);

		Snowfall::GetGameInstance().GetMeshManager().Render(buffer, pipeline, constants, desc, camera->LayerMask, {}, false);
		if (!camera->KeepInternal)
			CopyToSDR(buffer, camera);
	}
	for (CameraComponent *camera : m_scene->GetComponentManager().GetDeadComponents<CameraComponent>())
	{
		if (camera->HdrBuffer)
		{
			camera->HdrBuffer->Unload();
			delete camera->HdrBuffer;
		}
	}
	buffer.ExecuteCommands();
}

std::string CameraSystem::GetName()
{
	return "CameraSystem";
}

std::vector<std::string> CameraSystem::GetSystemsBefore()
{
	return { "TransformSystem" };
}

std::vector<std::string> CameraSystem::GetSystemsAfter()
{
	return {};
}

void CameraSystem::RenderSkybox(CommandBuffer& buffer, CameraComponent *camera, glm::mat4 view, TextureAsset *asset)
{
	Pipeline pipe;

	pipe.Shader = AssetManager::LocateAssetGlobal<ShaderAsset>("SkyboxShader").GetShaderVariant({});

	pipe.VertexStage.VertexArray = Snowfall::GetGameInstance().GetMeshManager().GetVertexArray();
	pipe.VertexStage.FrontFaceCulling = false;
	pipe.VertexStage.BackFaceCulling = false;

	pipe.FragmentStage.DrawTargets = { 0 };
	pipe.FragmentStage.DepthTest = false;
	pipe.FragmentStage.DepthMask = false;
	pipe.FragmentStage.Framebuffer = camera->HdrBuffer->GetFramebuffer();
	pipe.FragmentStage.Viewport = camera->Region.NoOffset();

	ShaderConstants consts;

	consts.AddConstant(0, view);
	consts.AddConstant(1, asset->GetTextureObject(), m_cubeSampler);

	buffer.BindPipelineCommand(pipe);
	buffer.BindConstantsCommand(consts);
	m_quad->DrawMeshDirect(buffer);
}

void CameraSystem::CopyToSDR(CommandBuffer& buffer, CameraComponent *camera)
{
	Pipeline pipe;

	pipe.Shader = AssetManager::LocateAssetGlobal<ShaderAsset>("CopyToSDR").GetShaderVariant({});

	pipe.VertexStage.VertexArray = Snowfall::GetGameInstance().GetMeshManager().GetVertexArray();
	pipe.VertexStage.FrontFaceCulling = false;
	pipe.VertexStage.BackFaceCulling = false;

	pipe.FragmentStage.DrawTargets = { 0 };
	pipe.FragmentStage.DepthTest = false;
	pipe.FragmentStage.DepthMask = false;

	if (camera->RenderTarget)
		pipe.FragmentStage.Framebuffer = camera->RenderTarget->GetFramebuffer();
	pipe.FragmentStage.Viewport = camera->Region;

	ShaderConstants consts;

	consts.AddConstant(0, camera->HdrBuffer->GetTexture(camera->colorAttachment)->GetTextureObject(), m_cubeSampler);

	buffer.BindPipelineCommand(pipe);
	buffer.BindConstantsCommand(consts);
	m_quad->DrawMeshDirect(buffer);
}
