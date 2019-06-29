#include "TestComponent.h"
#include <TransformComponent.h>
#include <Scene.h>

std::vector<SerializationField> ComponentDescriptor<TestComponent>::GetSerializationFields()
{
	return {
	};
}

void TestSystem::InitializeSystem(Scene& scene)
{
	m_scene = &scene;
}

void TestSystem::Update(float deltaTime)
{
	for (TestComponent *component : m_scene->GetComponentManager().GetComponents<TestComponent>())
	{
		TransformComponent *tcomp = component->Owner.GetComponent<TransformComponent>();
		tcomp->Rotation += glm::vec3(deltaTime * 200.f, 0, deltaTime * 200.f);
	}
}

std::string TestSystem::GetName()
{
	return "TestSystem";
}

std::vector<std::string> TestSystem::GetSystemsBefore()
{
	return std::vector<std::string>();
}

std::vector<std::string> TestSystem::GetSystemsAfter()
{
	return std::vector<std::string>();
}

bool TestSystem::IsMainThread()
{
	return false;
}
