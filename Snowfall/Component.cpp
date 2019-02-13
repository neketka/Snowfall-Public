#include "ECS.h"

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
	if (m_components.find(name) == m_components.end())
	{
		m_components.insert({ name, std::vector<Component *>() });
		m_deadComponents.insert({ name, std::vector<Component *>() });
	}
	std::vector<Component *>& comps = m_components[name];
	int size = m_pManager->GetComponentSize(name);
	char *memory = new char[size];
	memset(memory, 0, size);
	Component *comp = m_pManager->GetInitializer(name)(memory);
	comp->InternalName = name;
	comps.push_back(comp);
	return comp;
}

std::vector<Component *>& ComponentManager::GetComponents(std::string name)
{
	return m_components[name];
}

std::vector<Component *>& ComponentManager::GetDeadComponents(std::string name)
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

void ComponentManager::SerializeComponent(Component *component, IAssetStreamIO& stream)
{
	char *data = reinterpret_cast<char *>(component);
	int position = 0;
	for (SerializationField field : m_pManager->GetSerializationComponentFields(component->InternalName))
	{
		switch (field.Type)
		{
		case SerializationType::Entity:
			break;
		case SerializationType::String:
			break;
		case SerializationType::Asset:
			break;
		case SerializationType::EntityVector:
			break;
		case SerializationType::StringVector:
			break;
		case SerializationType::AssetVector:
			break;
		case SerializationType::ByValue:
			break;
		case SerializationType::ByValueVector:
			break;
		}
		position += field.Size;
	}
}

Component *ComponentManager::DeserializeComponent(IAssetStreamIO& stream)
{
	//char *data = reinterpret_cast<char *>(component);
	int position = 0;
	for (SerializationField field : m_pManager->GetSerializationComponentFields(""))
	{
		switch (field.Type)
		{
		case SerializationType::Entity:
			break;
		case SerializationType::String:
			break;
		case SerializationType::Asset:
			break;
		case SerializationType::EntityVector:
			break;
		case SerializationType::StringVector:
			break;
		case SerializationType::AssetVector:
			break;
		case SerializationType::ByValue:
			break;
		case SerializationType::ByValueVector:
			break;
		}
		position += field.Size;
	}
	return nullptr;
}
