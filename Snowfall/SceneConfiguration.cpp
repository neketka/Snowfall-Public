#include "SceneConfiguration.h"
#include "Entity.h"

SceneConfiguration::SceneConfiguration()
{
}

SceneConfiguration::~SceneConfiguration()
{
	for (auto entityPair : m_prototypes)
	{
		delete entityPair.second;
	}
}

Entity *SceneConfiguration::GetEntityPrototype(std::string name)
{
	auto entityIter = m_prototypes.find(name);
	if (entityIter == m_prototypes.end())
		return nullptr;
	return entityIter->second;
}

void SceneConfiguration::_AddEntityPrototypeWithName(Entity *entity)
{
	m_prototypes.insert_or_assign(entity->GetTypeName(), entity);
}
