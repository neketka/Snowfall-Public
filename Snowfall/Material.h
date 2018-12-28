#pragma once
#include "Shader.h"
#include "ShaderConstants.h"
#include "ShaderAsset.h"

class Material
{
public:
	ShaderAsset *MaterialShader;
	ShaderConstants Constants;
	bool operator==(const Material& other) const
	{
		return MaterialShader == other.MaterialShader;
	}
};