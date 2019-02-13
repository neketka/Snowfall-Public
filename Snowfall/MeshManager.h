#pragma once
#include "SubmitBuffer.h"
#include "VertexArray.h"
#include "Mesh.h"
#include "CommandBuffer.h"
#include "BoundingBox.h"
#include "Frustum.h"
#include "ECS.h"

#include <vector>
#include <string>

class GeometryHandle
{
public:
	GeometryHandle() : VertexAlloc(nullptr), IndexAlloc(nullptr) {}
	MemoryAllocation VertexAlloc;
	MemoryAllocation IndexAlloc;
};

class RendererStateChange
{
public:
	int Count;

	int IndirectOffset;
	int IndirectLength;

	PrimitiveType Type;
	Shader Shader;
	ShaderConstants Constants;
	ShaderDescriptor Descriptor;
	LayerMask LayerMask;
	int InstanceCount;
};

class MeshManager
{
public:
	MeshManager(int indirectCommands, int meshBufferVertexCount);
	~MeshManager();

	GeometryHandle CreateGeometry(int vlength, int ilength);
	void WriteGeometryVertices(GeometryHandle handle, RenderVertex *data, int offset, int size);
	void WriteGeometryIndices(GeometryHandle handle, int *data, int offset, int size);
	void EraseGeometry(GeometryHandle handle);

	void ClearData();
	void WriteIndirectCommandsToCullingPass(std::vector<GeometryHandle>& geometry, std::vector<BoundingBox>& boundingBoxes, RendererStateChange& state);
	void RunCullingPass(std::vector<Frustum> frusta);
	void Render(CommandBuffer& buffer, Pipeline p, ShaderConstants constants, ShaderDescriptor descriptor, LayerMask mask, bool overridePipeline);

	inline VertexArray GetVertexArray() { return m_defaultArray; }
private:
	std::vector<GeometryHandle> m_cullingGeometry;
	std::vector<BoundingBox> m_cullingBoxes;
	std::vector<RendererStateChange> m_stateChanges;
	VertexArray m_defaultArray;
	Buffer<DrawElementsIndirectCommand> m_indirect;
	Buffer<unsigned int> m_indirectAtomicCounter;
	SubmitBuffer<RenderVertex> m_vertexData;
	SubmitBuffer<int> m_indices;
};

