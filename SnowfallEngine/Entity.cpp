#include "ECS.h"
#include "Scene.h"

const std::string CHARS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
std::string generateUUID() 
{
	std::string uuid = std::string(36, ' ');
	int rnd = 0;
	int r = 0;

	uuid[8] = '-';
	uuid[13] = '-';
	uuid[18] = '-';
	uuid[23] = '-';

	uuid[14] = '4';

	for (int i = 0; i < 36; i++) {
		if (i != 8 && i != 13 && i != 18 && i != 14 && i != 23) {
			if (rnd <= 0x02) {
				rnd = 0x2000000 + (std::rand() * 0x1000000) | 0;
			}
			rnd >>= 4;
			uuid[i] = CHARS[(i == 19) ? ((rnd & 0xf) & 0x3) | 0x8 : rnd & 0xf];
		}
	}
	return uuid;
}

Entity EntityManager::CreateEntity(std::vector<std::string> components)
{
	EntityId id = generateUUID();
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
	EntityId newId = generateUUID();
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
	stream.WriteStream(reinterpret_cast<int *>(comps.size()), 1);
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