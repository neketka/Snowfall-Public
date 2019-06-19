#include "stdafx.h"

#include "PostProcessRenderSystem.h"
#include "Snowfall.h"

PostProcessRenderSystem::PostProcessRenderSystem()
{
}

PostProcessRenderSystem::~PostProcessRenderSystem()
{
	for (int i = 0; i < 4; ++i)
		m_samplers[i].Destroy();
}

void PostProcessRenderSystem::InitializeSystem(Scene& scene)
{
	for (int i = 0; i < 4; ++i)
	{
		m_samplers[i].SetWrapMode(TextureChannel::S, WrapMode::ClampToEdge);
		m_samplers[i].SetWrapMode(TextureChannel::T, WrapMode::ClampToEdge);
		m_samplers[i].SetWrapMode(TextureChannel::R, WrapMode::ClampToEdge);
		m_samplers[i].SetMinificationFilter(MinificationFilter::LinearMipmapNearest);
		m_samplers[i].SetMagnificationFilter(MagnificationFilter::Linear);
	}
	m_quad = &AssetManager::LocateAssetGlobal<MeshAsset>("FullScreenQuad");
	m_scene = &scene;
}

void PostProcessRenderSystem::Update(float deltaTime)
{
	CommandBuffer buffer;
	for (CameraComponent *camera : m_scene->GetComponentManager().GetComponents<CameraComponent>())
	{
		glm::mat4 invProj = glm::inverse(camera->ProjectionMatrix);
		for (MaterialAsset *asset : camera->PostProcessStack)
			RunPostprocess(buffer, camera, invProj, asset);
	}
	buffer.ExecuteCommands();
}

std::string PostProcessRenderSystem::GetName()
{
	return "PostProcessRenderSystem";
}

std::vector<std::string> PostProcessRenderSystem::GetSystemsBefore()
{
	return { "CameraSystem" };
}

std::vector<std::string> PostProcessRenderSystem::GetSystemsAfter()
{
	return std::vector<std::string>();
}

bool PostProcessRenderSystem::IsMainThread()
{
	return true;
}

void PostProcessRenderSystem::RunPostprocess(CommandBuffer& buffer, CameraComponent *camera, glm::mat4 invProj, MaterialAsset *postProcess)
{
	auto passes = postProcess->GetMaterial().MaterialShader->GetPreprocessed().GetPasses();
	auto downsamplePasses = postProcess->GetMaterial().MaterialShader->GetPreprocessed().GetDownsamplePasses();
	auto downscales = postProcess->GetMaterial().MaterialShader->GetPreprocessed().GetDownscalePasses();

	Pipeline pipe;

	pipe.VertexStage.VertexArray = Snowfall::GetGameInstance().GetMeshManager().GetVertexArray();
	pipe.VertexStage.FrontFaceCulling = false;
	pipe.VertexStage.BackFaceCulling = false;

	pipe.FragmentStage.DepthTest = false;
	pipe.FragmentStage.DepthMask = false;

	ShaderConstants consts;

	consts.AddConstant(0, Snowfall::GetGameInstance().GetTime());
	consts.AddConstant(1, invProj);
	consts.AddConstant(4, camera->HdrBuffer->GetTexture(4)->GetTextureObject(), m_samplers[0]);

	int passIndex = 0;
	int auxillaryAttachment = 0;
	for (std::string pass : passes)
	{
		ShaderConstants consts2;

		consts2.AddConstant(2, camera->HdrBuffer->GetTexture(camera->colorAttachment)->GetTextureObject(), m_samplers[1]);
		consts2.AddConstant(3, camera->HdrBuffer->GetTexture(auxillaryAttachment + 2)->GetTextureObject(), m_samplers[2]);

		camera->colorAttachment = 1 - camera->colorAttachment;
		auxillaryAttachment = 1 - auxillaryAttachment;

		std::vector<DownsamplePass>::iterator i;
		if ((i = std::find_if(downsamplePasses.begin(), downsamplePasses.end(),
			[passIndex](DownsamplePass t) { return t.Pass == passIndex && t.Level != 0; })) != downsamplePasses.end())
		{
			pipe.FragmentStage.Framebuffer = camera->Downsampled[i->Level - 1]->GetFramebuffer();
			pipe.FragmentStage.Viewport = camera->Region.NoOffset().Mipmapped(i->Level);
			pipe.FragmentStage.DrawTargets = { auxillaryAttachment };
			pipe.Shader = postProcess->GetMaterial().MaterialShader->GetShaderVariant({ pass, "DOWNSAMPLED" });
		}
		else
		{
			pipe.FragmentStage.Framebuffer = camera->HdrBuffer->GetFramebuffer();
			pipe.FragmentStage.Viewport = camera->Region.NoOffset();
			pipe.FragmentStage.DrawTargets = { camera->colorAttachment, auxillaryAttachment + 2 };
			pipe.Shader = postProcess->GetMaterial().MaterialShader->GetShaderVariant({ pass });
		}

		buffer.BindPipelineCommand(pipe);
		buffer.BindConstantsCommand(consts);
		buffer.BindConstantsCommand(consts2);
		m_quad->DrawMeshDirect(buffer);

		if (std::find(downscales.begin(), downscales.end(), passIndex) != downscales.end())
			camera->Downsampled[0]->GetTexture(auxillaryAttachment)->GetTextureObject().GenerateMipmap();
		++passIndex;
	}
}