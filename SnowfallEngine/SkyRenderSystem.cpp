#include "stdafx.h"
#include "SkyboxComponent.h"
#include "SkyRenderSystem.h"
#include "LightComponent.h"

void SkyRenderSystem::InitializeSystem(Scene& scene)
{
	m_scene = &scene;
}

void SkyRenderSystem::Update(float deltaTime)
{
	for (SkyboxComponent *comp : m_scene->GetComponentManager().GetComponents<SkyboxComponent>())
	{
		auto dir = m_scene->GetComponentManager().GetComponents<DirectionalLightComponent>();
		if (dir.size() == 0)
			continue;

		TransformComponent *tComp = dir[0]->Owner.GetComponent<TransformComponent>();
		if (!tComp)
			continue;

		if (comp->Cubemap)
		{
			if (!comp->renderTarget)
				comp->renderTarget = new RenderTargetAsset("", { comp->Cubemap }, {}, { TextureLayerAttachment(0, 0, -1) });

			CommandBuffer buffer;

			Pipeline pipe;

			pipe.Shader = AssetManager::LocateAssetGlobal<ShaderAsset>("AtmosphericSkyShader").GetShaderVariant({});

			pipe.VertexStage.VertexArray = Snowfall::GetGameInstance().GetMeshManager().GetVertexArray();
			pipe.VertexStage.FrontFaceCulling = false;
			pipe.VertexStage.BackFaceCulling = false;

			pipe.FragmentStage.DrawTargets = { 0 };
			pipe.FragmentStage.DepthTest = false;
			pipe.FragmentStage.DepthMask = false;
			pipe.FragmentStage.Framebuffer = comp->renderTarget->GetFramebuffer();
			pipe.FragmentStage.Viewport = IQuad2D(0, 0, comp->Cubemap->GetTextureObject().GetWidth(), comp->Cubemap->GetTextureObject().GetHeight());

			ShaderConstants consts;

			consts.AddConstant(0, -tComp->GlobalDirection);

			buffer.BindPipelineCommand(pipe);
			buffer.BindConstantsCommand(consts);
			buffer.DrawCommand(PrimitiveType::Triangles, 0, 36, 1, 0);

			buffer.ExecuteCommands();
		}
	}
}

std::string SkyRenderSystem::GetName()
{
	return "SkyRenderSystem";
}

std::vector<std::string> SkyRenderSystem::GetSystemsBefore()
{
	return std::vector<std::string>();
}

std::vector<std::string> SkyRenderSystem::GetSystemsAfter()
{
	return std::vector<std::string>();
}

bool SkyRenderSystem::IsMainThread()
{
	return true;
}
