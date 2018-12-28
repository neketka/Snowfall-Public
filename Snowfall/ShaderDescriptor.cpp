#include "ShaderDescriptor.h"

void ShaderDescriptor::AddUniformBuffer(TBuffer buffer, int bindingPoint)
{
	m_uniformBuffers.push_back(std::make_pair(buffer, bindingPoint));
}

void ShaderDescriptor::AddShaderStorageBuffer(TBuffer buffer, int bindingPoint)
{
	m_ssBuffers.push_back(std::make_pair(buffer, bindingPoint));
}

void ShaderDescriptor::AddAtomicCounterBuffer(TBuffer buffer, int bindingPoint)
{
	m_acBuffers.push_back(std::make_pair(buffer, bindingPoint));
}

void ShaderDescriptor::BindDescriptor()
{
	for (std::pair<TBuffer, int> uniformBuffer : m_uniformBuffers)
		glBindBufferBase(GL_UNIFORM_BUFFER, uniformBuffer.second, uniformBuffer.first.GetID());
	for (std::pair<TBuffer, int> ssBuffer : m_ssBuffers)
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssBuffer.second, ssBuffer.first.GetID());
	for (std::pair<TBuffer, int> acBuffer : m_acBuffers)
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, acBuffer.second, acBuffer.first.GetID());
}
