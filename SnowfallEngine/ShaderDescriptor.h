#pragma once
#include "Buffer.h"

#include "export.h"

class ShaderDescriptor
{
public:
	SNOWFALLENGINE_API void AddUniformBuffer(TBuffer buffer, int bindingPoint);
	SNOWFALLENGINE_API void AddShaderStorageBuffer(TBuffer buffer, int bindingPoint);
	SNOWFALLENGINE_API void AddAtomicCounterBuffer(TBuffer buffer, int bindingPoint);
	SNOWFALLENGINE_API void BindDescriptor();
private:
	std::vector<std::pair<TBuffer, int>> m_uniformBuffers;
	std::vector<std::pair<TBuffer, int>> m_ssBuffers;
	std::vector<std::pair<TBuffer, int>> m_acBuffers;
};

