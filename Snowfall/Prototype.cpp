#include "ECS.h"

std::vector<SerializationField>& PrototypeManager::GetSerializationComponentFields(std::string name)
{
	return m_componentDescriptions[name].SerializationFields;
}

int PrototypeManager::GetComponentSize(std::string name)
{
	return m_componentDescriptions[name].Size;
}

std::function<Component *(char*)> PrototypeManager::GetInitializer(std::string name)
{
	return m_componentDescriptions[name].InitializeComponent;
}