#include "TransformComponent.h"
#include "Scene.h"
#include <glm/gtx/transform.hpp>

std::vector<SerializationField> ComponentDescriptor<TransformComponent>::GetSerializationFields()
{
	return {
	};
}

void TransformSystem::InitializeSystem(Scene& scene)
{
	m_scene = &scene;
}

glm::mat4 CreateTransform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale)
{
	return glm::translate(pos) * glm::rotate(rot.y, glm::vec3(0, 1, 0)) *
		glm::rotate(rot.x, glm::vec3(1, 0, 0)) * glm::rotate(rot.z, glm::vec3(0, 0, 1)) * glm::scale(scale);;
}

void TransformRecursively(TransformComponent *component, long instant)
{
	if (component->transformInstant == instant || !(component->Enabled || (component->transformInstant != 0)))
		return;
	component->ModelMatrix = CreateTransform(component->Position, component->Rotation, component->Scale);
	component->transformInstant = instant;
	if (!component->Parent.IsNull())
	{
		TransformComponent *t = component->Parent.GetComponent<TransformComponent>();
		if (t->transformInstant < instant)
			TransformRecursively(t, instant);
		component->ModelMatrix = t->ModelMatrix * component->ModelMatrix;
	}
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