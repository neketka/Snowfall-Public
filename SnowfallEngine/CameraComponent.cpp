#include "stdafx.h"

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
		SerializationField("Enabled", SerializationType::ByValue, offsetof(CameraComponent, Enabled), sizeof(bool), InterpretValueAs::Bool),
		SerializationField("Region", SerializationType::ByValue, offsetof(CameraComponent, Region), sizeof(IQuad2D), InterpretValueAs::IVector2Array),
		SerializationField("LayerMask", SerializationType::ByValue, offsetof(CameraComponent, LayerMask), sizeof(LayerMask), InterpretValueAs::UInt64),
		SerializationField("ZNear", SerializationType::ByValue, offsetof(CameraComponent, ZNear), sizeof(float), InterpretValueAs::Float32),
		SerializationField("ZFar", SerializationType::ByValue, offsetof(CameraComponent, ZFar), sizeof(float), InterpretValueAs::Float32),
		SerializationField("FovY", SerializationType::ByValue, offsetof(CameraComponent, FovY), sizeof(float), InterpretValueAs::Float32),
		SerializationField("ViewportIndex", SerializationType::ByValue, offsetof(CameraComponent, ViewportIndex), sizeof(int), InterpretValueAs::Int32),
		SerializationField("KeepInternal", SerializationType::ByValue, offsetof(CameraComponent, KeepInternal), sizeof(bool), InterpretValueAs::Bool),
		SerializationField("RenderTarget", SerializationType::Asset, offsetof(CameraComponent, RenderTarget), 1, InterpretValueAs::RenderTargetAsset),
		SerializationField("PostProcessStack", SerializationType::AssetVector, offsetof(CameraComponent, PostProcessStack), 1, InterpretValueAs::MaterialAsset),
	};
}

CameraSystem::CameraSystem()
{
	m_quad = &AssetManager::LocateAssetGlobal<MeshAsset>("FullScreenQuad");

	m_sampler.SetWrapMode(TextureChannel::S, WrapMode::ClampToEdge);
	m_sampler.SetWrapMode(TextureChannel::T, WrapMode::ClampToEdge);
	m_sampler.SetWrapMode(TextureChannel::R, WrapMode::ClampToEdge);
	m_sampler.SetMinificationFilter(MinificationFilter::LinearMipmapLinear);
	m_sampler.SetMagnificationFilter(MagnificationFilter::Linear);

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
	for (int i = 0; i < 3; ++i)
		m_shadowSamplers[i].Destroy();
	m_sampler.Destroy();

	for (CameraComponent *camera : m_scene->GetComponentManager().GetComponents<CameraComponent>())
	{
		if (camera->HdrBuffer)
		{
			for (RenderTargetAsset *rasset : camera->Downsampled)
			{
				rasset->Unload();
				delete rasset;
			}
			camera->HdrBuffer->Unload();
			delete camera->HdrBuffer;
		}
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
		if (!camera->Enabled)
			continue;

		camera->colorAttachment = 0;
		if (camera->ViewportIndex != -1)
			camera->Region = Snowfall::GetGameInstance().GetViewport(camera->ViewportIndex);

		if (camera->oldRegion != camera->Region)
		{
			if (camera->HdrBuffer)
			{
				for (RenderTargetAsset *rasset : camera->Downsampled)
				{
					rasset->Unload();
					delete rasset;
				}
				camera->HdrBuffer->Unload();
				delete camera->HdrBuffer;
			}

			camera->HdrBuffer = new RenderTargetAsset("", {},
			{
					new TextureAsset("", TextureType::Texture2D,
				TextureInternalFormat::RGBA32F, camera->Region.Size.x, camera->Region.Size.y, 1, 1), //Color
					new TextureAsset("", TextureType::Texture2D, 
				TextureInternalFormat::RGBA32F, camera->Region.Size.x, camera->Region.Size.y, 1, 1),

					new TextureAsset("", TextureType::Texture2D,
				TextureInternalFormat::RGBA32F, camera->Region.Size.x, camera->Region.Size.y, 1, 5), //Auxillary
					new TextureAsset("", TextureType::Texture2D,
				TextureInternalFormat::RGBA32F, camera->Region.Size.x, camera->Region.Size.y, 1, 5),

					new TextureAsset("", TextureType::Texture2D,
				TextureInternalFormat::Depth24I, camera->Region.Size.x, camera->Region.Size.y, 1, 1), //Depth
			}, 
			{ 
				TextureLayerAttachment(0, 0, -1), 
				TextureLayerAttachment(1, 0, -1),
				
				TextureLayerAttachment(2, 0, -1),
				TextureLayerAttachment(3, 0, -1),

				TextureLayerAttachment(4, 0, -1)
				
			});

			for (int i = 0; i < 4; ++i)
			{
				camera->Downsampled[i] = new RenderTargetAsset("",
				{
					camera->HdrBuffer->GetTexture(2),
					camera->HdrBuffer->GetTexture(3)
				}, {},
				{
					TextureLayerAttachment(0, i + 1, -1),
					TextureLayerAttachment(1, i + 1, -1)
				}, false);
			}

			camera->oldRegion = camera->Region;
		}

		camera->colorAttachment = 0;

		Snowfall::GetGameInstance().GetMeshManager().RunCullingPass({});
		TransformComponent *transform = camera->Owner.GetComponent<TransformComponent>();

		camera->HdrBuffer->GetFramebuffer().ClearColor(0, glm::vec4(0, 0, 0, 1));
		camera->HdrBuffer->GetFramebuffer().ClearDepth(1);

		camera->ProjectionMatrix = glm::perspective(glm::radians(camera->FovY), static_cast<float>(camera->Region.Size.x) / static_cast<float>(camera->Region.Size.y), camera->ZNear, camera->ZFar);
		camera->ViewMatrix = glm::mat4(glm::mat3(transform->ModelMatrix)) * glm::translate(-transform->GlobalPosition);
		camera->UIProjectionMatrix = glm::ortho<float>(0, camera->Region.Size.x, 0, camera->Region.Size.y, -1, 1);

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
		pipeline.FragmentStage.DrawTargets = { camera->colorAttachment };
		pipeline.VertexStage.BackFaceCulling = true;

		ShaderConstants constants;
		constants.AddConstant(0, camera->ProjectionMatrix);
		constants.AddConstant(1, camera->ViewMatrix);
		constants.AddConstant(4, transform->GlobalPosition);
		constants.AddConstant(12, shadowRenderer->GetDirectionHighShadowTexture()->GetTextureObject(), m_shadowSamplers[0]);
		constants.AddConstant(13, shadowRenderer->GetFlatShadowTexture()->GetTextureObject(), m_shadowSamplers[1]);
		constants.AddConstant(14, shadowRenderer->GetCubeShadowTexture()->GetTextureObject(), m_shadowSamplers[2]);
		constants.AddConstant(15, Snowfall::GetGameInstance().GetTime());

		ShaderDescriptor desc;
		desc.AddShaderStorageBuffer(lights, 2);

		Snowfall::GetGameInstance().GetMeshManager().Render(buffer, pipeline, constants, desc, camera->LayerMask, {}, false, false);

	}

	for (CameraComponent *camera : m_scene->GetComponentManager().GetDeadComponents<CameraComponent>())
	{
		if (camera->HdrBuffer)
		{
			for (RenderTargetAsset *rasset : camera->Downsampled)
			{
				rasset->Unload();
				delete rasset;
			}
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
	consts.AddConstant(1, asset->GetTextureObject(), m_sampler);

	buffer.BindPipelineCommand(pipe);
	buffer.BindConstantsCommand(consts);
	m_quad->DrawMeshDirect(buffer);
}

