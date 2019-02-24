#include "CameraFlyComponent.h"

#include <Scene.h>
#include <TransformComponent.h>
#include <MeshComponent.h>
#include <Snowfall.h>
#include <InputManager.h>

std::vector<SerializationField> ComponentDescriptor<CameraFlyComponent>::GetSerializationFields()
{
	return {
	};
}

void CameraFlySystem::InitializeSystem(Scene& scene)
{
	m_scene = &scene;
}

void CameraFlySystem::Update(float deltaTime)
{
	static bool clicked = false;
	static bool locked = false;
	static float sens = 0.1f;
	static float speed = 2.0f;

	for (CameraFlyComponent *comp : m_scene->GetComponentManager().GetComponents<CameraFlyComponent>())
	{
		TransformComponent *transform = comp->Owner.GetComponent<TransformComponent>();

		if (Snowfall::GetGameInstance().GetInputManager().IsButtonDown(MouseButton::BUTTON_LEFT))
		{
			if (!clicked)
			{
				clicked = true;
				locked = !locked;
				Snowfall::GetGameInstance().GetInputManager().SetMouseLocked(locked);
				if (locked)
					Snowfall::GetGameInstance().GetInputManager().SetMousePosition(glm::vec2(transform->Rotation.y / sens, transform->Rotation.x / sens));
			}
		}
		else
			clicked = false;
		if (locked)
		{
			glm::vec2 rot = Snowfall::GetGameInstance().GetInputManager().GetMousePosition();
			transform->Rotation = glm::vec3(glm::clamp(rot.y * sens, -89.9f, 89.9f), rot.x * sens, 0);
			Snowfall::GetGameInstance().GetInputManager().SetMousePosition(glm::vec2(transform->Rotation.y / sens, transform->Rotation.x / sens));
		}

		glm::vec3 move = glm::vec3(0, 0, 0);
		if (Snowfall::GetGameInstance().GetInputManager().IsKeyDown(Key::KEY_W))
			move += transform->GlobalDirection;
		if (Snowfall::GetGameInstance().GetInputManager().IsKeyDown(Key::KEY_S))
			move -= transform->GlobalDirection;
		if (Snowfall::GetGameInstance().GetInputManager().IsKeyDown(Key::KEY_A))
			move += glm::cross(glm::vec3(0, 1, 0), transform->GlobalDirection);
		if (Snowfall::GetGameInstance().GetInputManager().IsKeyDown(Key::KEY_D))
			move -= glm::cross(glm::vec3(0, 1, 0), transform->GlobalDirection);
		if (glm::length(move) != 0)
			transform->Position += glm::normalize(move) * speed * deltaTime;
	}
}

std::string CameraFlySystem::GetName()
{
	return "CameraFlySystem";
}

std::vector<std::string> CameraFlySystem::GetSystemsBefore()
{
	return std::vector<std::string>();
}

std::vector<std::string> CameraFlySystem::GetSystemsAfter()
{
	return std::vector<std::string>();
}

bool CameraFlySystem::IsMainThread()
{
	return false;
}
