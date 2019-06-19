#include "stdafx.h"

#include "TransformComponent.h"
#include "Scene.h"
#include <glm/gtx/transform.hpp>

std::vector<SerializationField> ComponentDescriptor<TransformComponent>::GetSerializationFields()
{
	return {
		SerializationField("Parent", SerializationType::Entity, offsetof(TransformComponent, Parent), 1, InterpretValueAs::Entity),
		SerializationField("Position", SerializationType::ByValue, offsetof(TransformComponent, Position), sizeof(glm::vec3), InterpretValueAs::FVector3),
		SerializationField("Rotation", SerializationType::ByValue, offsetof(TransformComponent, Rotation), sizeof(glm::vec3), InterpretValueAs::FVector3),
		SerializationField("Scale", SerializationType::ByValue, offsetof(TransformComponent, Scale), sizeof(glm::vec3), InterpretValueAs::FVector3),
		SerializationField("Enabled", SerializationType::ByValue, offsetof(TransformComponent, Enabled), sizeof(bool), InterpretValueAs::Bool),
	};
}

void TransformSystem::InitializeSystem(Scene& scene)
{
	m_scene = &scene;
}

glm::mat4 CreateTransform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale)
{
	return glm::translate(pos) * glm::rotate(glm::radians(rot.z), glm::vec3(0, 0, 1)) *
		glm::rotate(glm::radians(rot.x), glm::vec3(1, 0, 0)) * glm::rotate(glm::radians(rot.y), glm::vec3(0, 1, 0)) * glm::scale(scale);;
}

void TransformRecursively(TransformComponent *component, long instant)
{
	if (component->transformInstant == instant || !(component->Enabled || (component->transformInstant != 0)))
		return;
	component->ModelMatrix = CreateTransform(component->Position, component->Rotation, component->Scale);
	component->transformInstant = instant;
	component->GlobalRotation = component->Rotation;
	if (!component->Parent.IsNull())
	{
		TransformComponent *t = component->Parent.GetComponent<TransformComponent>();
		if (t->transformInstant < instant)
			TransformRecursively(t, instant);
		component->ModelMatrix = t->ModelMatrix * component->ModelMatrix;
		component->GlobalRotation += t->Rotation;
	}
	component->GlobalPosition = component->ModelMatrix[3];
	component->GlobalDirection = -glm::vec3(component->ModelMatrix[0][2], component->ModelMatrix[1][2], component->ModelMatrix[2][2]);
}

void TransformSystem::Update(float deltaTime)
{
	for (TransformComponent *transformComp : m_scene->GetComponentManager().GetComponents<TransformComponent>())
	{
		TransformRecursively(transformComp, m_transformInstant);
	}
	++m_transformInstant;
}

std::string TransformSystem::GetName()
{
	return "TransformSystem";
}

std::vector<std::string> TransformSystem::GetSystemsBefore()
{
	return {};
}

std::vector<std::string> TransformSystem::GetSystemsAfter()
{
	return {};
}