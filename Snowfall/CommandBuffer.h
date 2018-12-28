#pragma once
#include <functional>

#include "Pipeline.h"
#include "ShaderConstants.h"
#include "ShaderDescriptor.h"
#include "OcclusionQuery.h"

enum class PrimitiveType
{
	Triangles = GL_TRIANGLES
};

class SyncObject
{
private:
	GLsync m_sync;
};

class CommandBuffer
{
public:
	void ClearColorFramebufferCommand(int attachment, glm::vec4 color);
	void ClearDepthFramebufferCommand(float value);

	void BeginOcclusionQuery(OcclusionQuery query);
	void EndOcclusionQuery(OcclusionQuery query);

	void BeginConditionalRendering(OcclusionQuery query);
	void EndConditionalRendering(); 

	void BeginTransformFeedback(PrimitiveType type);
	void EndTransformFeedback();

	void BindPixelUnpackBufferCommand(TBuffer buffer);
	void BindPixelPackBufferCommand(TBuffer buffer);

	void BindIndirectCommandBufferCommand(TBuffer buffer);

	void BindComputeShaderCommand(Shader shader);
	void BindPipelineCommand(Pipeline pipeline);
	void BindConstantsCommand(ShaderConstants constants);
	void BindDescriptorCommand(ShaderDescriptor descriptor);

	void DrawIndexedCommand(PrimitiveType type, int count, int instances, int baseIndex, int baseVertex, int baseInstance);
	void DrawCommand(PrimitiveType type, int first, int count, int instances, int baseInstance);
	void DrawIndexedIndirectCommand(PrimitiveType type, int cmdCount, int stride);
	void DrawIndirectCommand(PrimitiveType type, int cmdCount, int stride);
	void DispatchCommand(int x, int y, int z);
	
	//void ExecuteCommandBufferCommand(CommandBuffer buffer);

	void ExecuteCommands();

	void ClearCommandBuffer();
private:
	std::vector<std::function<void()>> m_commands;

	std::vector<ShaderConstants> m_constants;
	std::vector<ShaderDescriptor> m_descriptors;
	std::vector<Pipeline> m_pipelines;
	std::vector<CommandBuffer> m_subBuffers;
};

