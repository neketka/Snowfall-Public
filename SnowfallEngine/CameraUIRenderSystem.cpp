#include "stdafx.h"
#include "CameraUIRenderSystem.h"
#include "UIContext.h"

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
		if (!camera->Enabled)
			continue;

		UIContext *context = Snowfall::GetGameInstance().GetUIContext(comp->ContextName);


		UIRenderer& uiRenderer = Snowfall::GetGameInstance().GetUIRenderer();
		uiRenderer.StartRenderPass(camera->HdrBuffer->GetFramebuffer(), camera->colorAttachment, camera->Region, camera->UIProjectionMatrix);

		if (context)
		{
			if (comp->oldRegion != camera->Region)
			{
				comp->oldRegion = camera->Region;
				context->SetBounds(camera->Region);
			}
			context->UpdateContext(uiRenderer, deltaTime, comp->IsSpectator);
		}

		uiRenderer.RenderText(&AssetManager::LocateAssetGlobal<FontAsset>("ArialBasic"), glm::vec2(5.f, camera->Region.Size.y - 20.f), 16.f, glm::vec4(1, 1, 1, 1),
			"FPS: " + std::to_string(static_cast<int>(std::floor(Snowfall::GetGameInstance().GetFPS()))));

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
