#include "stdafx.h"


std::vector<SerializationField>& PrototypeManager::GetSerializationComponentFields(std::string name)
{
	return m_componentDescriptions[name].SerializationFields;
}

int PrototypeManager::GetComponentSize(std::string name)
{
	return m_componentDescriptions[name].Size;
}

InitializeComponentFunc PrototypeManager::GetInitializer(std::string name)
{
	return m_componentDescriptions[name].InitializeComponent;
}

CopyComponentFunc PrototypeManager::GetCopy(std::string name)
{
	return m_componentDescriptions[name].CopyComponent;
}
