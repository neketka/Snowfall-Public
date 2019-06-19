#include "stdafx.h"

#include "SkyboxComponent.h"

std::vector<SerializationField> ComponentDescriptor<SkyboxComponent>::GetSerializationFields()
{
	return {
		SerializationField("Cubemap", SerializationType::Asset, offsetof(SkyboxComponent, Cubemap), sizeof(TextureAsset *), InterpretValueAs::TextureAsset),
		SerializationField("Enabled", SerializationType::ByValue, offsetof(SkyboxComponent, Enabled), sizeof(bool), InterpretValueAs::Bool)
	};
}