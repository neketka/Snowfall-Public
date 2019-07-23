#include "stdafx.h"

#include "MeshManager.h"

MeshManager::MeshManager(int indirectCommands, int meshBufferVertexCount)
	: m_indirect(indirectCommands, BufferOptions(false, false, false, false, true, false)),
	m_indirectAtomicCounter(1, BufferOptions(true, true, true, true, true, false))
{

	AllocateGeometryBuffer<RenderVertex>(0, VertexArray({ BufferStructure({
		Attribute(0, 0, 3, 12, true), //Pos
		Attribute(1, 12, 4, 16, true), //Color
		Attribute(2, 28, 3, 12, true), //Normal
		Attribute(3, 40, 3, 12, true), //Tangent
		Attribute(4, 52, 2, 8, true), //UV
	}) }), meshBufferVertexCount * 0.5 / sizeof(RenderVertex), meshBufferVertexCount * 0.5 / sizeof(int));

	m_indirectAtomicCounter.MapBuffer(0, 2, MappingOptions(true, true, true, true, false, false, false, true));
}

MeshManager::~MeshManager()
{
	m_indirect.Destroy();
	m_indirectAtomicCounter.Destroy();
	for (auto kp : m_geometryBuffers)
		delete kp.second;
}

GeometryHandle MeshManager::CreateGeometry(int vlength, int ilength, int buffer)
{
	GeometryHandle handle = m_geometryBuffers[buffer]->CreateGeometry(vlength, ilength);
	handle.Buffer = buffer;
	return handle;
}

void MeshManager::WriteGeometryIndices(GeometryHandle handle, int *data, int offset, int size)
{
	m_geometryBuffers[handle.Buffer]->WriteGeometryIndices(handle, data, offset, size);
}

void MeshManager::EraseGeometry(GeometryHandle handle)
{
	m_geometryBuffers[handle.Buffer]->EraseGeometry(handle);
}

void MeshManager::ClearData()
{
	m_cullingGeometry.clear();
	m_cullingBoxes.clear();
	m_stateChanges.clear();
}

void MeshManager::WriteIndirectCommandsToCullingPass(std::vector<GeometryHandle>& geometry, std::vector<BoundingBox>& boundingBoxes, RendererStateChange& state)
{
	state.ObjectCount = geometry.size();
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
		for (int i = 0; i < change.ObjectCount; ++i)
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
				command.BaseVertex = handle.VertexAlloc.GetPosition();
				command.Count = handle.IndexAlloc.GetLength();
				commands.push_back(command);
			}
		}
		change.indirectOffset = passed;
		change.indirectLength = objectsPassed;

		current += change.ObjectCount;
		passed += objectsPassed;
	}
	m_indirect.CopyData(commands, 0);
	//*counter = passed;
}

void MeshManager::Render(CommandBuffer& buffer, Pipeline p, ShaderConstants& constants, ShaderDescriptor& descriptor, LayerMask mask, std::set<std::string> specializations, bool overrideShader, bool overrideConstants)
{
	for (RendererStateChange& change : m_stateChanges)
	{
		p.VertexStage.VertexArray = m_geometryBuffers[change.GeometryBuffer]->GetVertexArray();
		if (!(mask & change.LayerMask))
			continue;
		std::set spec = std::set(change.Specializations);
		spec.insert(specializations.begin(), specializations.end());
		if (!overrideShader)
			p.Shader = change.Shader->GetShaderVariant(spec);

		buffer.BindPipelineCommand(p);

		buffer.BindConstantsCommand(constants); //TODO: add combine function in the future instead of replacing buffers
		if (!overrideConstants)
			buffer.BindConstantsCommand(change.Constants);

		buffer.BindDescriptorCommand(descriptor);
		buffer.BindDescriptorCommand(change.Descriptor);

		buffer.BindIndirectCommandBufferCommand(m_indirect);
		buffer.DrawIndexedIndirectCommand(change.Type, change.indirectOffset * sizeof(DrawElementsIndirectCommand), change.indirectLength, 0);
	}
}

