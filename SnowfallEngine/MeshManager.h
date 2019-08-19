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

	int Buffer = 0;
};

class RendererStateChange
{
public:
	int ObjectCount;
	int GeometryBuffer = 0;

	int indirectOffset;
	int indirectLength;

	PrimitiveType Type;
	ShaderAsset *Shader;
	std::set<std::string> Specializations;
	
	ShaderConstants Constants;
	ShaderDescriptor Descriptor;
	LayerMask LayerMask;
	int InstanceCount;
};

class TGeometryBuffer
{
public:
	virtual ~TGeometryBuffer() { }
	virtual GeometryHandle CreateGeometry(int vlength, int ilength) = 0;
	virtual VertexArray& GetVertexArray() = 0;
	virtual void WriteGeometryVertices(GeometryHandle handle, void *data, int offset, int size) = 0;
	virtual void WriteGeometryIndices(GeometryHandle handle, int *data, int offset, int size) = 0;
	virtual void EraseGeometry(GeometryHandle handle) = 0;
};

template<class T>
class GeometryBuffer : public TGeometryBuffer
{
public:
	GeometryBuffer(VertexArray arr, int vMax, int iMax) : m_vertex(vMax), m_indices(iMax), m_arr(arr)
	{
		arr.SetBuffer(0, m_vertex.GetBuffer());
		arr.SetIndexBuffer(m_indices.GetBuffer());
	}

	virtual ~GeometryBuffer() override
	{
		m_arr.Destroy();
		m_vertex.Destroy();
		m_indices.Destroy();
	}

	virtual GeometryHandle CreateGeometry(int vlength, int ilength) override
	{
		GeometryHandle handle;
		handle.VertexAlloc = m_vertex.Allocate(vlength);
		handle.IndexAlloc = m_indices.Allocate(ilength);
		return handle;
	}

	virtual VertexArray& GetVertexArray() override
	{
		return m_arr;
	}

	virtual void WriteGeometryVertices(GeometryHandle handle, void *data, int offset, int size) override
	{
		m_vertex.UploadData(handle.VertexAlloc, reinterpret_cast<T *>(data), offset, size);
	}

	virtual void WriteGeometryIndices(GeometryHandle handle, int *data, int offset, int size) override
	{
		m_indices.UploadData(handle.IndexAlloc, data, offset, size);
	}

	virtual void EraseGeometry(GeometryHandle handle) override
	{
		m_vertex.Release(handle.VertexAlloc);
		m_indices.Release(handle.IndexAlloc);
	}
private:
	VertexArray m_arr;
	SubmitBuffer<T> m_vertex;
	SubmitBuffer<int> m_indices;
};

class MeshManager
{
public:
	SNOWFALLENGINE_API MeshManager(int indirectCommands, int meshBufferVertexCount);
	SNOWFALLENGINE_API ~MeshManager();

	SNOWFALLENGINE_API GeometryHandle CreateGeometry(int vlength, int ilength, int buffer=0);
	template<class T>
	void WriteGeometryVertices(GeometryHandle handle, T *data, int offset, int size)
	{
		m_geometryBuffers[handle.Buffer]->WriteGeometryVertices(handle, data, offset, size);
	}
	SNOWFALLENGINE_API void WriteGeometryIndices(GeometryHandle handle, int *data, int offset, int size);
	SNOWFALLENGINE_API void EraseGeometry(GeometryHandle handle);
	
	template<class T>
	void AllocateGeometryBuffer(int id, VertexArray arr, int vMax, int iMax)
	{
		m_geometryBuffers[id] = new GeometryBuffer<T>(arr, vMax, iMax);
	}

	SNOWFALLENGINE_API void ClearData();
	SNOWFALLENGINE_API void WriteIndirectCommandsToCullingPass(std::vector<GeometryHandle>& geometry, std::vector<BoundingBox>& boundingBoxes, RendererStateChange& state);
	SNOWFALLENGINE_API void RunCullingPass(std::vector<Frustum> frusta);
	SNOWFALLENGINE_API void Render(CommandBuffer& buffer, Pipeline p, ShaderConstants& constants, ShaderDescriptor& descriptor, LayerMask mask, std::set<std::string> specializations, bool overrideShader=false, bool overrideConstants=false);
	SNOWFALLENGINE_API std::vector<BoundingBox> GetLayerBounds(LayerMask mask);

	inline VertexArray GetVertexArray(int index = 0) { return m_geometryBuffers[index]->GetVertexArray(); }
private:
	std::vector<GeometryHandle> m_cullingGeometry;
	std::vector<BoundingBox> m_cullingBoxes;
	std::vector<RendererStateChange> m_stateChanges;
	std::map<int, TGeometryBuffer *> m_geometryBuffers;
	Buffer<DrawElementsIndirectCommand> m_indirect;
	Buffer<unsigned int> m_indirectAtomicCounter;
};

