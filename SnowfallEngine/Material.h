#pragma once
#include "Shader.h"
#include "ShaderConstants.h"
#include "ShaderAsset.h"

class Material
{
public:
	Material() {}
	Material(ShaderAsset *shader) : MaterialShader(shader) {}

	ShaderAsset *MaterialShader;
	ShaderConstants Constants;
	int PerObjectParameterCount;

	bool operator==(const Material& other) const
	{
		return MaterialShader == other.MaterialShader;
	}
};