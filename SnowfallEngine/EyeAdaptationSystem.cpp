#include "stdafx.h"
#include "EyeAdaptationSystem.h"
#include "CameraComponent.h"

void EyeAdaptationSystem::InitializeSystem(Scene& scene)
{
	m_scene = &scene;
}

void EyeAdaptationSystem::Update(float deltaTime)
{
	for (EyeAdaptationComponent *eyeAdapt : m_scene->GetComponentManager().GetComponents<EyeAdaptationComponent>())
	{
		CameraComponent *camera = eyeAdapt->Owner.GetComponent<CameraComponent>();
		if (!camera)
			continue;
		int levels = glm::max<int>(5, glm::ceil(glm::log2(glm::max<float>(camera->Region.Size.x, camera->Region.Size.y))));
		if (eyeAdapt->oldRegion != camera->Region && camera->HdrBuffer)
		{
			if (eyeAdapt->renderTarget)
			{
				eyeAdapt->renderTarget->Unload();
				eyeAdapt->renderTargetEase->Unload();
			}

			eyeAdapt->mipView = new TextureAsset("", TextureType::Texture2D, TextureInternalFormat::RGBA32F, 1, 1, 1, 1);

			eyeAdapt->renderTarget = new RenderTargetAsset("", {
				camera->HdrBuffer->GetTexture(2),
			}, {
			}, {
				TextureLayerAttachment(0, 0, -1),
			}, false);

			eyeAdapt->renderTargetEase = new RenderTargetAsset("", {}, {
				eyeAdapt->mipView
			}, {
				TextureLayerAttachment(0, 0, -1),
			}, false);

			eyeAdapt->sampler.SetWrapMode(TextureChannel::S, WrapMode::ClampToEdge);
			eyeAdapt->sampler.SetWrapMode(TextureChannel::T, WrapMode::ClampToEdge);
			eyeAdapt->sampler.SetWrapMode(TextureChannel::R, WrapMode::ClampToEdge);
			eyeAdapt->sampler.SetMinificationFilter(MinificationFilter::LinearMipmapLinear);
			eyeAdapt->sampler.SetMagnificationFilter(MagnificationFilter::Linear);

			eyeAdapt->sampler2.SetWrapMode(TextureChannel::S, WrapMode::ClampToEdge);
			eyeAdapt->sampler2.SetWrapMode(TextureChannel::T, WrapMode::ClampToEdge);
			eyeAdapt->sampler2.SetWrapMode(TextureChannel::R, WrapMode::ClampToEdge);
			eyeAdapt->sampler2.SetMinificationFilter(MinificationFilter::LinearMipmapLinear);
			eyeAdapt->sampler2.SetMagnificationFilter(MagnificationFilter::Linear);

			eyeAdapt->oldRegion = camera->Region;
		}

		if (!eyeAdapt->renderTarget)
			continue;

		if (eyeAdapt->TonemappingMaterial)
		{
			eyeAdapt->TonemappingMaterial->GetBaseProperties().Constants.AddConstant(5, eyeAdapt->mipView->GetTextureObject(), eyeAdapt->sampler);
			eyeAdapt->TonemappingMaterial->GetBaseProperties().Constants.AddConstant(6, glm::vec4(eyeAdapt->MinLuma, eyeAdapt->MaxLuma, eyeAdapt->ExposureBias, eyeAdapt->KeyValue));
		}

		if (eyeAdapt->BloomMaterial)
		{
			eyeAdapt->BloomMaterial->GetBaseProperties().Constants.AddConstant(5, eyeAdapt->mipView->GetTextureObject(), eyeAdapt->sampler);
			eyeAdapt->TonemappingMaterial->GetBaseProperties().Constants.AddConstant(6, glm::vec4(eyeAdapt->MinLuma, eyeAdapt->MaxLuma, eyeAdapt->ExposureBias, eyeAdapt->KeyValue));
		}

		Pipeline pipe;

		pipe.VertexStage.VertexArray = Snowfall::GetGameInstance().GetMeshManager().GetVertexArray();
		pipe.VertexStage.FrontFaceCulling = false;
		pipe.VertexStage.BackFaceCulling = false;

		pipe.FragmentStage.DepthTest = false;
		pipe.FragmentStage.DepthMask = false;

		pipe.FragmentStage.Framebuffer = eyeAdapt->renderTarget->GetFramebuffer();
		pipe.FragmentStage.Viewport = eyeAdapt->oldRegion;
		pipe.FragmentStage.DrawTargets = { 0 };
		pipe.Shader = AssetManager::LocateAssetGlobal<ShaderAsset>("EyeAdaptationStore").GetShaderVariant({});

		ShaderConstants consts;

		consts.AddConstant(0, camera->HdrBuffer->GetTexture(camera->colorAttachment)->GetTextureObject(), eyeAdapt->sampler);

		CommandBuffer buffer;

		buffer.BindPipelineCommand(pipe);
		buffer.BindConstantsCommand(consts);

		AssetManager::LocateAssetGlobal<MeshAsset>("FullScreenQuad").DrawMeshDirect(buffer);

		buffer.ExecuteCommands();

		eyeAdapt->renderTarget->GetTexture(0)->GetTextureObject().GenerateMipmap();

		RunEasingFunction(eyeAdapt, deltaTime, levels - 1);
		//eyeAdapt->renderTarget->GetTexture(0)->GetTextureObject().CopyPixels(eyeAdapt->mipView->GetTextureObject(), 0, 0, 0, levels - 1, 0, 0, 0, 0, 1, 1, 1);
	}
}

void EyeAdaptationSystem::RunEasingFunction(EyeAdaptationComponent *comp, float deltaT, int level)
{
	Pipeline pipe;

	pipe.VertexStage.VertexArray = Snowfall::GetGameInstance().GetMeshManager().GetVertexArray();
	pipe.VertexStage.FrontFaceCulling = false;
	pipe.VertexStage.BackFaceCulling = false;

	pipe.FragmentStage.DepthTest = false;
	pipe.FragmentStage.DepthMask = false;

	pipe.FragmentStage.Framebuffer = comp->renderTargetEase->GetFramebuffer();
	pipe.FragmentStage.Viewport = IQuad2D(0, 0, 1, 1);
	pipe.FragmentStage.DrawTargets = { 0 };
	pipe.Shader = AssetManager::LocateAssetGlobal<ShaderAsset>("EyeAdaptationEase").GetShaderVariant({});

	ShaderConstants consts;

	consts.AddConstant(0, comp->mipView->GetTextureObject(), comp->sampler);
	consts.AddConstant(1, comp->renderTarget->GetTexture(0)->GetTextureObject(), comp->sampler2);
	consts.AddConstant(2, glm::vec4(comp->MinLuma, comp->MaxLuma, deltaT, comp->EaseConstant));
	consts.AddConstant(3, level);

	CommandBuffer buffer;

	buffer.BindPipelineCommand(pipe);
	buffer.BindConstantsCommand(consts);

	buffer.TextureBarrier();

	AssetManager::LocateAssetGlobal<MeshAsset>("FullScreenQuad").DrawMeshDirect(buffer);

	buffer.ExecuteCommands();
}

std::string EyeAdaptationSystem::GetName()
{
	return "EyeAdaptationSystem";
}

std::vector<std::string> EyeAdaptationSystem::GetSystemsBefore()
{
	return { };
}

std::vector<std::string> EyeAdaptationSystem::GetSystemsAfter()
{
	return { };
}

bool EyeAdaptationSystem::IsMainThread()
{
	return true;
}

std::vector<SerializationField> ComponentDescriptor<EyeAdaptationComponent>::GetSerializationFields()
{
	return {
	};
}