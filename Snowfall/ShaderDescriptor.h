#pragma once
#include "Buffer.h"

class ShaderDescriptor
{
public:
	void AddUniformBuffer(TBuffer buffer, int bindingPoint);
	void AddShaderStorageBuffer(TBuffer buffer, int bindingPoint);
	void AddAtomicCounterBuffer(TBuffer buffer, int bindingPoint);
	void BindDescriptor();
private:
	std::vector<std::pair<TBuffer, int>> m_uniformBuffers;
	std::vector<std::pair<TBuffer, int>> m_ssBuffers;
	std::vector<std::pair<TBuffer, int>> m_acBuffers;
};

