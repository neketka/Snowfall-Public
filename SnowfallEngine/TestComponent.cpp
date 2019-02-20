#include "TestComponent.h"
#include "TransformComponent.h"

std::vector<SerializationField> ComponentDescriptor<TestComponent>::GetSerializationFields()
{
	return {
	};
}

void TestSystem::InitializeSystem(EntityManager *eManager, ComponentManager *cManager, EventManager *evManager, PrototypeManager *pManager)
{
	m_cManager = cManager;
}

float t = 0;
void TestSystem::Update(float deltaTime)
{
	bool doDelete = t > 1.0;
	for (TestComponent *comp : m_cManager->GetComponents<TestComponent>())
	{
		TransformComponent *transform = comp->Owner.GetComponent<TransformComponent>();
		transform->Rotation += glm::vec3(deltaTime, deltaTime * 0.5f, 0);
		if (doDelete)
		{
			doDelete = false;
			//comp->Owner.Kill();
			t = 0;
		}
	}
	t += deltaTime;
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
