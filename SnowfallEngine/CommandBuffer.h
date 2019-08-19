#pragma once
#include <functional>

#include "Pipeline.h"
#include "ShaderConstants.h"
#include "ShaderDescriptor.h"
#include "OcclusionQuery.h"

#include "export.h"

enum class PrimitiveType
{
	Triangles = GL_TRIANGLES
};

enum class MemoryBarrierType : unsigned
{
	VertexAttribArray = GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT,
	IndexBuffer = GL_ELEMENT_ARRAY_BARRIER_BIT,
	Uniform = GL_UNIFORM_BARRIER_BIT,
	TextureFetch = GL_TEXTURE_FETCH_BARRIER_BIT,
	ShaderImageAccess = GL_SHADER_IMAGE_ACCESS_BARRIER_BIT,
	CommandBarrier = GL_COMMAND_BARRIER_BIT,
	PixelBuffer = GL_PIXEL_BUFFER_BARRIER_BIT,
	TextureUpdate = GL_TEXTURE_UPDATE_BARRIER_BIT,
	BufferUpdate = GL_BUFFER_UPDATE_BARRIER_BIT,
	QueryBuffer = GL_QUERY_BUFFER_BARRIER_BIT,
	ClientMappedBuffer = GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT,
	Framebuffer = GL_FRAMEBUFFER_BARRIER_BIT,
	TransformFeedback = GL_TRANSFORM_FEEDBACK_BARRIER_BIT,
	AtomicCounter = GL_ATOMIC_COUNTER_BARRIER_BIT,
	ShaderStorageBuffer = GL_SHADER_STORAGE_BARRIER_BIT,
	All = GL_ALL_BARRIER_BITS
};

class SyncObject
{
private:
	GLsync m_sync;
};

class CommandBuffer
{
public:
	CommandBuffer() {
		m_commands.reserve(40);
		m_descriptors.reserve(20);
		m_pipelines.reserve(20);
		m_constants.reserve(20);
	}
	SNOWFALLENGINE_API void ClearColorFramebufferCommand(int attachment, glm::vec4 color);
	SNOWFALLENGINE_API void ClearDepthFramebufferCommand(float value);
	SNOWFALLENGINE_API void ClearAllBuffers();

	SNOWFALLENGINE_API void BeginOcclusionQuery(OcclusionQuery query);
	SNOWFALLENGINE_API void EndOcclusionQuery(OcclusionQuery query);

	SNOWFALLENGINE_API void BeginConditionalRendering(OcclusionQuery query);
	SNOWFALLENGINE_API void EndConditionalRendering();

	SNOWFALLENGINE_API void BeginTransformFeedback(PrimitiveType type);
	SNOWFALLENGINE_API void EndTransformFeedback();

	SNOWFALLENGINE_API void BindPixelUnpackBufferCommand(TBuffer buffer);
	SNOWFALLENGINE_API void BindPixelPackBufferCommand(TBuffer buffer);

	SNOWFALLENGINE_API void BindIndirectCommandBufferCommand(TBuffer buffer);

	SNOWFALLENGINE_API void BindComputeShaderCommand(Shader shader);
	SNOWFALLENGINE_API void BindPipelineCommand(Pipeline pipeline);
	SNOWFALLENGINE_API void BindConstantsCommand(ShaderConstants constants);
	SNOWFALLENGINE_API void BindDescriptorCommand(ShaderDescriptor descriptor);

	SNOWFALLENGINE_API void DrawIndexedCommand(PrimitiveType type, int count, int instances, int baseIndex, int baseVertex, int baseInstance);
	SNOWFALLENGINE_API void DrawCommand(PrimitiveType type, int first, int count, int instances, int baseInstance);
	SNOWFALLENGINE_API void DrawIndexedIndirectCommand(PrimitiveType type, int offset, int cmdCount, int stride);
	SNOWFALLENGINE_API void DrawIndirectCommand(PrimitiveType type, int offset, int cmdCount, int stride);
	SNOWFALLENGINE_API void DispatchCommand(int x, int y, int z);

	SNOWFALLENGINE_API void MemoryBarrier(MemoryBarrierType type);
	SNOWFALLENGINE_API void TextureBarrier();
	
	//void ExecuteCommandBufferCommand(CommandBuffer buffer);

	SNOWFALLENGINE_API void ExecuteCommands();

	SNOWFALLENGINE_API void ClearCommandBuffer();
private:
	std::vector<std::function<void()>> m_commands;

	std::vector<ShaderConstants> m_constants;
	std::vector<ShaderDescriptor> m_descriptors;
	std::vector<Pipeline> m_pipelines;
	std::vector<CommandBuffer> m_subBuffers;
};

