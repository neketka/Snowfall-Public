#pragma once
#include "TextureAsset.h"
#include "ECS.h"

class SkyboxComponent : public Component
{
public:
	TextureAsset *Cubemap;
	bool Enabled;
};