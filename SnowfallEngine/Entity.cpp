#include "stdafx.h"

#include "Scene.h"
#include "UUID.h"

Entity EntityManager::CreateEntity(std::vector<std::string> components)
{
	EntityId id = GenerateUUID();
	m_entities.insert({ id, std::vector<Component *>() });
	std::vector<Component *>& comps = m_entities[id];
	Entity e = Entity(id, this);
	for (std::string name : components)
	{
		Component *comp = m_scene.GetComponentManager().CreateComponent(name);
		comp->Owner = e;
		comps.push_back(comp);
	}
	return e;
}

Component *EntityManager::GetComponent(EntityId id, std::string component)
{
	for (auto comp : m_entities[id])
	{
		if (comp->InternalName == component)
			return comp;
	}
	return nullptr;
}

std::vector<Component*>& EntityManager::GetComponents(EntityId id)
{
	return m_entities[id];
}

void EntityManager::KillEntity(EntityId id)
{
	for (Component *c : m_entities[id])
		m_scene.GetComponentManager().DeleteComponent(c);
	m_entities.erase(id);
}

Entity EntityManager::CloneEntity(EntityId id)
{
	EntityId newId = GenerateUUID();
	m_entities.insert({ newId, std::vector<Component *>() });
	std::vector<Component *>& oldComps = m_entities[id];
	std::vector<Component *>& comps = m_entities[newId];
	Entity e = Entity(newId, this);
	for (Component *comp : oldComps)
	{
		Component *cloned = m_scene.GetComponentManager().CloneComponent(comp);
		cloned->Owner = e;
		comps.push_back(cloned);
	}
	return e;
}

void EntityManager::AddComponent(EntityId id, std::string component)
{
	Component *comp = m_scene.GetComponentManager().CreateComponent(component);
	comp->Owner = Entity(id, this);
	comp->InternalName = std::string(component);
	m_entities[id].push_back(comp);
}

void EntityManager::RemoveComponent(EntityId id, std::string component)
{
	int i = 0;
	for (Component *c : m_entities[id])
	{
		if (c->InternalName == component)
			break;
		++i;
	}
	std::vector<Component *>& comps = m_entities[id];
	comps.erase(comps.begin() + i);
}

void EntityManager::SerializeEntity(EntityId id, IAssetStreamIO& stream)
{
	stream.WriteString(id);
	std::vector<Component *>& comps = m_entities[id];
	int size = comps.size();
	stream.WriteStream(&size, 1);
	for (Component *c : comps)
	{
		m_scene.GetComponentManager().SerializeComponent(c, stream);
	}
}

std::vector<Entity> EntityManager::LoadEntities(int count, IAssetStreamIO& stream)
{
	std::vector<Entity> loaded;
	for (int i = 0; i < count; ++i)
	{
		EntityId id = stream.ReadString();
		m_entities.insert_or_assign( id, std::vector<Component *>() );
		std::vector<Component *>& comps = m_entities[id];

		Entity e = Entity(id, this);

		int cCount = 0;
		stream.ReadStream(&cCount, 1);

		for (int j = 0; j < cCount; ++j)
		{
			Component *c = m_scene.GetComponentManager().DeserializeComponent(stream);
			c->Owner = e;
			comps.push_back(c);
		}

		loaded.push_back(e);
	}
	return loaded;
}

int EntityManager::GetEntityCount()
{
	return m_entities.size();
}