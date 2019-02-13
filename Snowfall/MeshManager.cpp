#include "MeshManager.h"

MeshManager::MeshManager(int indirectCommands, int meshBufferVertexCount)
	: m_indirect(indirectCommands, BufferOptions(false, false, false, false, true, false)),
	m_indirectAtomicCounter(1, BufferOptions(true, true, true, true, true, false)),
	m_vertexData(meshBufferVertexCount),
	m_indices(meshBufferVertexCount * 1.1f)
{
	m_defaultArray = VertexArray({ BufferStructure({
		Attribute(0, 0, 3, 12, true), //Pos
		Attribute(1, 12, 4, 16, true), //Color
		Attribute(2, 28, 3, 12, true), //Normal
		Attribute(3, 40, 3, 12, true), //Tangent
		Attribute(4, 52, 2, 8, true), //UV
	}) });

	m_indirectAtomicCounter.MapBuffer(0, 2, MappingOptions(true, true, true, true, false, false, false, true));

	m_defaultArray.SetBuffer(0, m_vertexData.GetBuffer());
	m_defaultArray.SetIndexBuffer(m_indices.GetBuffer());
}

MeshManager::~MeshManager()
{
	m_indirect.Destroy();
	m_indirectAtomicCounter.Destroy();
	m_vertexData.Destroy();
	m_indices.Destroy();
}

GeometryHandle MeshManager::CreateGeometry(int vlength, int ilength)
{
	GeometryHandle handle;
	handle.VertexAlloc = m_vertexData.Allocate(vlength);
	handle.IndexAlloc = m_indices.Allocate(ilength);
	return handle;
}

void MeshManager::WriteGeometryVertices(GeometryHandle handle, RenderVertex *data, int offset, int size)
{
	m_vertexData.UploadData(handle.VertexAlloc, data, offset, size);
}

void MeshManager::WriteGeometryIndices(GeometryHandle handle, int *data, int offset, int size)
{
	m_indices.UploadData(handle.IndexAlloc, data, offset, size);
}

void MeshManager::EraseGeometry(GeometryHandle handle)
{
	m_vertexData.Release(handle.VertexAlloc);
	m_indices.Release(handle.IndexAlloc);
}

void MeshManager::ClearData()
{
	m_cullingGeometry.clear();
	m_cullingBoxes.clear();
	m_stateChanges.clear();
}

void MeshManager::WriteIndirectCommandsToCullingPass(std::vector<GeometryHandle>& geometry, std::vector<BoundingBox>& boundingBoxes, RendererStateChange& state)
{
	state.Count = geometry.size();
	m_cullingGeometry.insert(m_cullingGeometry.end(), geometry.begin(), geometry.end());
	m_cullingBoxes.insert(m_cullingBoxes.end(), boundingBoxes.begin(), boundingBoxes.end());
	m_stateChanges.push_back(state);
}

void MeshManager::RunCullingPass(std::vector<Frustum> frusta)
{
	/*m_indirectAtomicCounter.GetMappedPointer()[0] = 0;
	unsigned int *counter = m_indirectAtomicCounter.GetMappedPointer();*/
	std::vector<DrawElementsIndirectCommand> commands;
	int current = 0;
	int passed = 0;
	for (RendererStateChange& change : m_stateChanges)
	{
		int objectsPassed = 0;
		for (int i = 0; i < change.Count; ++i)
		{
			GeometryHandle handle = m_cullingGeometry[i + current];
			BoundingBox box = m_cullingBoxes[i + current];
			if (true) //TODO: add frustum culling
			{
				++objectsPassed;
				DrawElementsIndirectCommand command;
				command.FirstIndex = handle.IndexAlloc.GetPosition();
				command.InstanceCount = change.InstanceCount;
				command.BaseInstance = 0;
				command.BaseVertex = 0;
				command.Count = handle.IndexAlloc.GetLength();
				commands.push_back(command);
			}
		}
		change.IndirectOffset = passed;
		change.IndirectLength = objectsPassed;

		current += change.Count;
		passed += objectsPassed;
	}
	m_indirect.CopyData(commands, 0);
	//*counter = passed;
}

void MeshManager::Render(CommandBuffer& buffer, Pipeline p, ShaderConstants constants, ShaderDescriptor descriptor, LayerMask mask, bool overrideShader)
{
	p.VertexStage.VertexArray = m_defaultArray;
	for (RendererStateChange& change : m_stateChanges)
	{
		if (!(mask & change.LayerMask))
			continue;
		if (!overrideShader)
			p.Shader = change.Shader;

		buffer.BindPipelineCommand(p);

		buffer.BindConstantsCommand(constants); //TODO: add combine function to support Vulkan in the future instead of replacing buffers
		buffer.BindConstantsCommand(change.Constants);

		buffer.BindDescriptorCommand(descriptor);
		buffer.BindDescriptorCommand(change.Descriptor);

		buffer.BindIndirectCommandBufferCommand(m_indirect);
		buffer.DrawIndexedIndirectCommand(change.Type, change.IndirectOffset * sizeof(DrawElementsIndirectCommand), change.IndirectLength, 0);
	}
}

