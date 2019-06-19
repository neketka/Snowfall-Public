#pragma once
#include "SubmitBuffer.h"
#include "VertexArray.h"
#include "Mesh.h"
#include "CommandBuffer.h"
#include "BoundingBox.h"
#include "Frustum.h"
#include "ECS.h"
#include "ShaderAsset.h"

#include <vector>
#include <string>

#include "export.h"

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
	ShaderAsset *Shader;
	std::set<std::string> Specializations;
	
	ShaderConstants Constants;
	ShaderDescriptor Descriptor;
	LayerMask LayerMask;
	int InstanceCount;
};

class MeshManager
{
public:
	SNOWFALLENGINE_API MeshManager(int indirectCommands, int meshBufferVertexCount);
	SNOWFALLENGINE_API ~MeshManager();

	SNOWFALLENGINE_API GeometryHandle CreateGeometry(int vlength, int ilength);
	SNOWFALLENGINE_API void WriteGeometryVertices(GeometryHandle handle, RenderVertex *data, int offset, int size);
	SNOWFALLENGINE_API void WriteGeometryIndices(GeometryHandle handle, int *data, int offset, int size);
	SNOWFALLENGINE_API void EraseGeometry(GeometryHandle handle);

	SNOWFALLENGINE_API void ClearData();
	SNOWFALLENGINE_API void WriteIndirectCommandsToCullingPass(std::vector<GeometryHandle>& geometry, std::vector<BoundingBox>& boundingBoxes, RendererStateChange& state);
	SNOWFALLENGINE_API void RunCullingPass(std::vector<Frustum> frusta);
	SNOWFALLENGINE_API void Render(CommandBuffer& buffer, Pipeline p, ShaderConstants& constants, ShaderDescriptor& descriptor, LayerMask mask, std::set<std::string> specializations, bool overrideShader=false, bool overrideConstants=false);

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

