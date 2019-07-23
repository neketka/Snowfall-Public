#pragma once
#include "Buffer.h"

#include "export.h"

class ShaderDescriptor
{
public:
	SNOWFALLENGINE_API void AddUniformBuffer(TBuffer buffer, int bindingPoint, int offset = 0);
	SNOWFALLENGINE_API void AddShaderStorageBuffer(TBuffer buffer, int bindingPoint, int offset = 0);
	SNOWFALLENGINE_API void AddAtomicCounterBuffer(TBuffer buffer, int bindingPoint, int offset = 0);
	SNOWFALLENGINE_API void BindDescriptor();
private:
	std::vector<std::tuple<TBuffer, int, int>> m_uniformBuffers;
	std::vector<std::tuple<TBuffer, int, int>> m_ssBuffers;
	std::vector<std::tuple<TBuffer, int, int>> m_acBuffers;
};

