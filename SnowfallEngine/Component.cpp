#include "ECS.h"
#include "Snowfall.h"
#include "Scene.h"

ComponentManager::~ComponentManager()
{
	for (auto compPairs : m_components)
	{
		for (Component *comp : compPairs.second)
		{
			char *original = reinterpret_cast<char *>(comp);
			comp->~Component();
			delete[] original;
		}
	}
}

Component *ComponentManager::CreateComponent(std::string name)
{
	auto compsIter = m_components.end();
	if ((compsIter = m_components.find(name)) == m_components.end())
	{
		m_components.insert({ name, std::vector<Component *>() });
		m_deadComponents.insert({ name, std::vector<Component *>() });
		compsIter = m_components.find(name);
	}
	std::vector<Component *>& comps = compsIter->second;
	Component *comp = CreateComponentRaw(name);
	comps.push_back(comp);
	return comp;
}

Component *ComponentManager::CreateComponentRaw(std::string name)
{
	int size = Snowfall::GetGameInstance().GetPrototypeManager().GetComponentSize(name);
	char *memory = new char[size];
	memset(memory, 0, size);
	Component *comp = Snowfall::GetGameInstance().GetPrototypeManager().GetInitializer(name)(memory);
	comp->InternalName = name;
	return comp;
}

std::vector<Component *> ComponentManager::GetComponents(std::string name)
{
	return m_components[name];
}

std::vector<Component *> ComponentManager::GetDeadComponents(std::string name)
{
	return m_deadComponents[name];
}

void ComponentManager::CleanDeadComponents()
{
	for (int i = 0; i < m_deadAddedLast; ++i)
	{
		Component *c = m_toKill.front();
		std::vector<Component *>& comps = m_deadComponents[c->InternalName];
		auto iter = std::find(comps.begin(), comps.end(), c);
		comps.erase(iter);

		c->~Component();
		char *original = reinterpret_cast<char *>(c);
		delete[] original;

		m_toKill.pop();
	}
	m_deadAddedLast = m_deadAddedNext;
	m_deadAddedNext = 0;
}

void ComponentManager::DeleteComponent(Component *component)
{
	std::vector<Component *>& comps = m_components[component->InternalName];
	auto iter = std::find(comps.begin(), comps.end(), component);
	m_deadComponents[component->InternalName].push_back(*iter);
	m_toKill.push(*iter);
	++m_deadAddedNext;
	comps.erase(iter);
}

void ComponentManager::DeleteComponentRaw(Component *component)
{
	component->~Component();
	delete[] component;
}

void ComponentManager::SerializeComponent(Component *component, IAssetStreamIO& stream)
{
	char *data = reinterpret_cast<char *>(component);
	stream.WriteString(component->InternalName);
	stream.WriteString(component->Owner.GetId());
	int position = 0;
	for (SerializationField field : Snowfall::GetGameInstance().GetPrototypeManager().GetSerializationComponentFields(component->InternalName))
	{
		switch (field.Type)
		{
			case SerializationType::Entity:
			{
				Entity *e = reinterpret_cast<Entity *>(data + position);
				stream.WriteString(e->GetId());
				position += sizeof(Entity);
				break;
			}
			case SerializationType::String:
			{
				stream.WriteString(data + position);
				position += sizeof(std::string);
				break;
			}
			case SerializationType::Asset:
			{
				IAsset *asset = reinterpret_cast<IAsset *>(data + position);
				if (asset)
					stream.WriteString(asset->GetPath());
				else
					stream.WriteString("null");
				position += sizeof(IAsset *);
				break;
			}
			case SerializationType::EntityVector:
			{
				std::vector<Entity> *ents = reinterpret_cast<std::vector<Entity> *>(data + position);
				int size = static_cast<int>(ents->size());
				stream.WriteStream(&size, 1);
				for (Entity e : *ents)
					stream.WriteString(e.GetId());
				position += sizeof(std::vector<Entity>);
				break;
			}
			case SerializationType::StringVector:
			{
				std::vector<std::string> *strs = reinterpret_cast<std::vector<std::string> *>(data + position);
				int size = static_cast<int>(strs->size());
				stream.WriteStream(&size, 1);
				for (std::string str : *strs)
					stream.WriteString(str);
				position += sizeof(std::vector<std::string>);
				break;
			}
			case SerializationType::AssetVector:
			{
				std::vector<IAsset *> *asts = reinterpret_cast<std::vector<IAsset *> *>(data + position);
				int size = static_cast<int>(asts->size());
				stream.WriteStream(&size, 1);
				for (IAsset *asset : *asts)
				{
					if (asset)
						stream.WriteString(asset->GetPath());
					else
						stream.WriteString("null");
				}
				position += sizeof(std::vector<IAsset *>);
			}
				break;
			case SerializationType::ByValue:
			{
				stream.WriteStreamBytes(data + position, field.Size);
				position += field.Size;
				break;
			}
			case SerializationType::ByValueVector:
			{
				std::vector<char> *vals = reinterpret_cast<std::vector<char> *>(data + position);
				stream.WriteStreamBytes(vals->data(), vals->size());
				position += sizeof(std::vector<char>);
				break;
			}
			case SerializationType::NonSerializable:
			{
				position += field.Size;
				break;
			}
		}
	}
}

Component *ComponentManager::CloneComponent(Component *component)
{
	std::vector<Component *>& comps = m_components[component->InternalName];
	int size = Snowfall::GetGameInstance().GetPrototypeManager().GetComponentSize(component->InternalName);
	char *memory = new char[size];
	memset(memory, 0, size);
	Component *comp = Snowfall::GetGameInstance().GetPrototypeManager().GetCopy(component->InternalName)(component, memory);
	comps.push_back(comp);
	return comp;
}

Component *ComponentManager::DeserializeComponent(IAssetStreamIO& stream)
{
	Component *comp = DeserializeComponentRaw(stream);

	auto compsIter = m_components.end();
	if ((compsIter = m_components.find(comp->InternalName)) == m_components.end())
	{
		m_components.insert({ comp->InternalName, std::vector<Component *>() });
		m_deadComponents.insert({ comp->InternalName, std::vector<Component *>() });
		compsIter = m_components.find(comp->InternalName);
	}

	std::vector<Component *>& comps = compsIter->second;
	comps.push_back(comp);
	return comp;
}

Component *ComponentManager::DeserializeComponentRaw(IAssetStreamIO& stream)
{
	std::string internalName = stream.ReadString();
	std::string owner = stream.ReadString();
	Component *comp = CreateComponentRaw(internalName);
	char *data = reinterpret_cast<char *>(comp);
	int position = 0;
	for (SerializationField field : Snowfall::GetGameInstance().GetPrototypeManager().GetSerializationComponentFields(internalName))
	{
		switch (field.Type)
		{
			case SerializationType::Entity:
			{
				*reinterpret_cast<Entity *>(data + position) = Entity(stream.ReadString(), &m_scene.GetEntityManager());
				position += sizeof(Entity);
				break;
			}
			case SerializationType::String:
			{
				*reinterpret_cast<std::string *>(data + position) = stream.ReadString();
				position += sizeof(std::string);
				break;
			}
			case SerializationType::Asset:
			{
				std::string path = stream.ReadString();
				IAsset **asset = reinterpret_cast<IAsset **>(data + position);
				if (path == "null")
					*asset = nullptr;
				else
					*asset = &AssetManager::LocateAssetGlobal<IAsset>(path);
				position += sizeof(IAsset *);
				break;
			}
			case SerializationType::EntityVector:
			{
				std::vector<Entity> *ents = reinterpret_cast<std::vector<Entity> *>(data + position);
				int size = 0;
				stream.ReadStream(&size, 1);
				for (int i = 0; i < size; ++i)
					ents->push_back(Entity(stream.ReadString(), &m_scene.GetEntityManager()));
				position += sizeof(std::vector<Entity>);
				break;
			}
			case SerializationType::StringVector:
			{
				std::vector<std::string> *strs = reinterpret_cast<std::vector<std::string> *>(data + position);
				int size = 0;
				stream.ReadStream(&size, 1);
				for (int i = 0; i < size; ++i)
					strs->push_back(stream.ReadString());
				position += sizeof(std::vector<std::string>);
				break;
			}
			case SerializationType::AssetVector:
			{
				std::vector<IAsset *> *assets = reinterpret_cast<std::vector<IAsset *> *>(data + position);
				int size = 0;
				stream.ReadStream(&size, 1);
				for (int i = 0; i < size; ++i)
				{
					std::string path = stream.ReadString();
					if (path == "null")
						assets->push_back(nullptr);
					else
						assets->push_back(&AssetManager::LocateAssetGlobal<IAsset>(path));
				}
				position += sizeof(std::vector<IAsset *>);
				break;
			}
			case SerializationType::ByValue:
			{
				stream.ReadStreamBytes(data + position, field.Size);
				position += field.Size;
				break;
			}
			case SerializationType::ByValueVector:
			{
				int size = 0;
				stream.ReadStream(&size, 1);
				std::vector<char> *dat = reinterpret_cast<std::vector<char> *>(data + position);
				dat->resize(size);
				stream.ReadStreamBytes(dat->data(), size);
				position += sizeof(std::vector<char>);
				break;
			}
			case SerializationType::NonSerializable:
			{
				position += field.Size;
				break;
			}
		}
	}
	return comp;
}
