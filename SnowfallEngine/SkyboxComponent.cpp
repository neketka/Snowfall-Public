#include "SkyboxComponent.h"

std::vector<SerializationField> ComponentDescriptor<SkyboxComponent>::GetSerializationFields()
{
	return {
		SerializationField("Cubemap", SerializationType::Asset, sizeof(TextureAsset *), InterpretValueAs::TextureAsset),
		SerializationField("Enabled", SerializationType::ByValue, sizeof(bool), InterpretValueAs::Bool)
	};
}