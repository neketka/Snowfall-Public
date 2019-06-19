#include "stdafx.h"
#include "CameraUIRenderSystem.h"

std::vector<SerializationField> ComponentDescriptor<CameraUIRenderComponent>::GetSerializationFields()
{
	return {
		SerializationField("ContextName", SerializationType::ByValue, offsetof(CameraUIRenderComponent, ContextName), sizeof(std::string), InterpretValueAs::String),
		SerializationField("IsSpectator", SerializationType::ByValue, offsetof(CameraUIRenderComponent, IsSpectator), sizeof(bool), InterpretValueAs::Bool)
	};
}

CameraUIRenderSystem::CameraUIRenderSystem()
{
}

void CameraUIRenderSystem::InitializeSystem(Scene& scene)
{
	m_scene = &scene;
}

void CameraUIRenderSystem::Update(float deltaTime)
{
	for (CameraUIRenderComponent *comp : m_scene->GetComponentManager().GetComponents<CameraUIRenderComponent>())
	{
		CameraComponent *camera = comp->Owner.GetComponent<CameraComponent>();
		if (!camera)
			continue;

		UIRenderer& uiRenderer = Snowfall::GetGameInstance().GetUIRenderer();

		uiRenderer.StartRenderPass(camera->HdrBuffer->GetFramebuffer(), camera->colorAttachment, camera->Region, camera->UIProjectionMatrix);
		uiRenderer.RenderText(&AssetManager::LocateAssetGlobal<FontAsset>("ArialBasic"), glm::vec2(5.f, camera->Region.Size.y - 40.f), 36.f, glm::vec4(1, 1, 1, 1),
			"FPS: " + std::to_string(static_cast<int>(std::floor(Snowfall::GetGameInstance().GetFPS()))));
		uiRenderer.RenderTexture(Quad2D(0, 0, 1, 1), Quad2D(100, 100, 300, 300), &AssetManager::LocateAssetGlobal<TextureAsset>("fortnite"), glm::vec4(1));
		uiRenderer.RenderRectangle(Quad2D(400, 0, 200, 200), glm::vec4(0, 0, 0, 1), glm::vec4(1), 5, true);
		uiRenderer.RenderRectangle(Quad2D(400, 200, 200, 200), &AssetManager::LocateAssetGlobal<TextureAsset>("coolBorder"), 20, glm::vec4(1), glm::vec4(1, 1, 1, 0.3f), true);

		uiRenderer.EndRenderPass();

		camera->colorAttachment = uiRenderer.GetCurrentDrawBuffer();
	}
}

std::string CameraUIRenderSystem::GetName()
{
	return "CameraUIRenderSystem";
}

std::vector<std::string> CameraUIRenderSystem::GetSystemsBefore()
{
	return { };
}

std::vector<std::string> CameraUIRenderSystem::GetSystemsAfter()
{
	return { "CameraViewportRenderSystem" };
}
