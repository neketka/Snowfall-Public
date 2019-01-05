#include "MeshRenderer.h"
#include "Entity.h"
#include "ShaderConstants.h"
#include "ShaderDescriptor.h"

MeshRenderer::MeshRenderer()
{
}

MeshRenderer::~MeshRenderer()
{
	for (StaticBatch& batch : m_staticBatches)
	{
		for (EntityRenderHandle *handle : batch.EntityHandles)
			delete handle;

		batch.AttributeArray.Destroy();
		batch.BatchBuffer.Destroy();
		batch.IndexBuffer.Destroy();
	}

	for (DynamicBatch& batch : m_dynamicBatches) 
	{
		for (EntityRenderHandle *handle : batch.EntityHandles)
			delete handle;

		batch.AttributeArray.Destroy();
		batch.BatchBuffer.Destroy();
		batch.IndexBuffer.Destroy();
		batch.TransformBuffer.Destroy();
		if (batch.HasParameters)
			batch.ParameterBuffer.Destroy();
	}

	for (EntityRenderHandle *handle : m_unbatched)
		delete handle;
}

void MeshRenderer::UpdateEntities()
{
	scanUnbatched();
	for (StaticBatch& batch : m_staticBatches)
		updateBatch(batch);
	for (DynamicBatch& batch : m_dynamicBatches)
		updateBatch(batch);
}

void MeshRenderer::Render(ICamera& camera, CommandBuffer& buffer)
{
	for (StaticBatch& batch : m_staticBatches)
		if (camera.GetLayerMask() & batch.LayerMask)
			renderBatch(batch, camera, buffer);
	for (DynamicBatch& batch : m_dynamicBatches)
		if (camera.GetLayerMask() & batch.LayerMask)
			renderBatch(batch, camera, buffer);
}

void MeshRenderer::RenderReplaced(ICamera& camera, Shader & shader, ShaderConstants & constants, ShaderDescriptor & descriptor, CommandBuffer & buffer)
{
}

EntityRenderHandle& MeshRenderer::CreateRenderEntity(EntityOptions options)
{
	EntityRenderHandle *handle = new EntityRenderHandle(options);
	m_unbatched.push_back(handle);
	return *handle;
}

VertexArray MeshRenderer::createStaticVertexFormat()
{
	return VertexArray({ BufferStructure({
		Attribute(0, 0, 3, 12, true), //Pos
		Attribute(1, 12, 4, 16, true), //Color
		Attribute(2, 28, 3, 12, true), //Normal
		Attribute(3, 40, 3, 12, true), //Tangent
		Attribute(4, 52, 2, 8, true), //UV
	}) });
}

VertexArray MeshRenderer::createDynamicVertexFormat()
{
	return VertexArray({ BufferStructure({
		Attribute(0, 0, 3, 12, true), //Pos
		Attribute(1, 12, 4, 16, true), //Color
		Attribute(2, 28, 3, 12, true), //Normal
		Attribute(3, 40, 3, 12, true), //Tangent
		Attribute(4, 52, 2, 8, true), //UV
	}), BufferStructure({
		Attribute(5, 0, 1, 4, false) //ObjectId
	}) });
}

void MeshRenderer::scanUnbatched()
{
	std::vector<int> toRemove;
	int index = 0;
	for (EntityRenderHandle *handle : m_unbatched)
	{
		if (handle->m_options.Instanced)
		{
		}
		else if (handle->m_options.Dynamic)
		{
			if (handle->m_mesh && handle->m_material)
			{
				toRemove.push_back(index);
				bool foundBatch = false;
				for (DynamicBatch& batch : m_dynamicBatches)
				{
					if (handle->m_options.Unbatched)
						break;
					if (*batch.Material == *handle->m_material && batch.EntityHandles.size() < MaxEntitiesPerDynamicBatch && batch.LayerMask == handle->m_mask)
					{
						batch.EntityHandles.push_back(handle);
						handle->m_objectId = batch.FreeIds.back();
						batch.FreeIds.pop_back();
						foundBatch = true;
					}
				}
				if (!foundBatch)
				{
					m_dynamicBatches.push_back(DynamicBatch());

					DynamicBatch& batch = m_dynamicBatches.back();
					batch.AttributeArray = createDynamicVertexFormat();
					batch.BatchBuffer = SubmitBuffer<RenderVertex>(0);
					batch.ObjectIdBuffer = Buffer<int>(0, BufferOptions(false, false, false, false, true, false));
					batch.IndexBuffer = Buffer<int>(0, BufferOptions(false, false, false, false, true, false));
					batch.TransformBuffer = Buffer<glm::mat4>(MaxEntitiesPerDynamicBatch * 2, BufferOptions(false, false, false, false, true, false));
					batch.Material = handle->m_material;
					batch.HasParameters = batch.Material->PerObjectParameterCount > 0;
					batch.LayerMask = handle->m_mask;
					if (batch.HasParameters)
						batch.ParameterBuffer = Buffer<glm::vec4>(MaxEntitiesPerDynamicBatch * batch.Material->PerObjectParameterCount,
							BufferOptions(false, false, false, false, true, false));
					batch.EntityHandles.push_back(handle);
					handle->m_objectId = 0;
					for (int i = MaxEntitiesPerDynamicBatch - 1; i > 0; --i)
						batch.FreeIds.push_back(i);
				}
			}
		}
		else
		{
			if (handle->m_mesh && handle->m_material)
			{
				toRemove.push_back(index);
				bool foundBatch = false;
				for (StaticBatch& batch : m_staticBatches)
				{
					if (handle->m_options.Unbatched)
						break;
					if (*batch.Material == *handle->m_material)
					{
						batch.EntityHandles.push_back(handle);
						foundBatch = true;
					}
				}
				if (!foundBatch)
				{
					m_staticBatches.push_back(StaticBatch());

					StaticBatch& batch = m_staticBatches.back();
					batch.AttributeArray = createStaticVertexFormat();
					batch.BatchBuffer = SubmitBuffer<RenderVertex>(0);
					batch.IndexBuffer = Buffer<int>(0, BufferOptions(false, false, false, false, true, false));
					batch.Material = handle->m_material;
					batch.EntityHandles.push_back(handle);
				}
			}
		}
		++index;
	}
	std::sort(toRemove.begin(), toRemove.end(), std::greater<>());
	for (int index : toRemove)
	{
		m_unbatched[index] = m_unbatched.back();
		m_unbatched.pop_back();
	}
}


void MeshRenderer::renderBatch(Batch& batch, ICamera& camera, CommandBuffer& cmdBuffer)
{
	Pipeline pipeline;
	ShaderConstants constants = ShaderConstants(batch.Material->Constants);
	ShaderDescriptor descriptor;

	std::string paramsDef = "";
	bool hasParams = false;
	if (batch.Dynamic)
	{
		DynamicBatch& d_batch = static_cast<DynamicBatch&>(batch);
		descriptor.AddUniformBuffer(d_batch.TransformBuffer, 0);
		paramsDef = "OBJECT_PARAMS " + std::to_string(d_batch.Material->PerObjectParameterCount);
		hasParams = d_batch.HasParameters;
		if (hasParams)
			descriptor.AddUniformBuffer(d_batch.ParameterBuffer, 1);
	}
	std::string dynamicDef = "MAX_DYNAMIC_OBJECTS " + std::to_string(MaxEntitiesPerDynamicBatch);

	pipeline.Shader = batch.Material->MaterialShader->GetShaderVariant({ batch.Dynamic ? "DYNAMIC" : "STATIC", dynamicDef, paramsDef, hasParams ? "HAS_PARAMS" : "NO_PARAMS" });
	pipeline.VertexStage.VertexArray = batch.AttributeArray;
	pipeline.FragmentStage.DepthTest = true;
	pipeline.FragmentStage.Framebuffer = camera.GetRenderTarget();
	pipeline.FragmentStage.Viewport = camera.GetRegion();
	pipeline.FragmentStage.DrawTargets = { 0 };

	constants.AddConstant(0, camera.GetProjectionMatrix());
	constants.AddConstant(1, camera.GetViewMatrix());

	cmdBuffer.BindPipelineCommand(pipeline);
	cmdBuffer.BindConstantsCommand(constants);
	cmdBuffer.BindDescriptorCommand(descriptor);
	cmdBuffer.DrawIndexedCommand(PrimitiveType::Triangles, batch.IndexBuffer.GetLength(), 1, 0, 0, 0);
}

EntityRenderHandle::EntityRenderHandle(EntityOptions options) : m_options(options), m_allocation(nullptr)
{
}

void EntityRenderHandle::SetMesh(Mesh& mesh)
{
	m_rebuildData = true;
	m_mesh = &mesh;
}

void EntityRenderHandle::SetMaterial(Material& material)
{
	m_material = &material;
	m_params.resize(material.PerObjectParameterCount);
}

void EntityRenderHandle::SetTransform(glm::mat4 transform)
{
	m_transform = transform;
}

void EntityRenderHandle::SetMaterialParameter(int index, glm::vec4 param)
{
	m_params[index] = param;
}

void EntityRenderHandle::SetLayerMask(LayerMask mask)
{
	m_mask = mask;
}

void EntityRenderHandle::Delete()
{
	m_stagedForRemoval = true;
}

void MeshRenderer::updateBatch(Batch& batch)
{
	bool rebuildBatchBuffer = false;
	bool rebuildIndexBuffer = false;
	int vertices = 0;
	int indices = 0;

	std::vector<int> toRemove;

	std::vector<glm::mat4> transformMats;
	std::vector<glm::vec4> perObjectParams;
	DynamicBatch *d_batch = nullptr;

	if (batch.Dynamic)
	{
		d_batch = static_cast<DynamicBatch *>(&batch);
		transformMats.resize(d_batch->TransformBuffer.GetLength());
		perObjectParams.resize(d_batch->ParameterBuffer.GetLength());
	}

	int index = 0;
	for (EntityRenderHandle *handle : batch.EntityHandles)
	{
		int vcount = static_cast<int>(handle->m_mesh->Vertices.size());
		vertices += vcount;
		indices += static_cast<int>(handle->m_mesh->Indices.size());
		if (rebuildBatchBuffer)
			continue;
		if (batch.Dynamic)
		{
			transformMats[handle->m_objectId] = handle->m_transform;
			transformMats[handle->m_objectId + MaxEntitiesPerDynamicBatch] = glm::mat4(glm::inverse(glm::transpose(glm::mat3(handle->m_transform))));
			if (d_batch->HasParameters)
				std::memcpy(&perObjectParams[handle->m_objectId * batch.Material->PerObjectParameterCount], handle->m_params.data(), handle->m_params.size() * sizeof(glm::vec4));
		}
		if (handle->m_rebuildData && !rebuildBatchBuffer)
		{
			rebuildIndexBuffer = true;

			if (handle->m_allocation.Allocated())
				batch.BatchBuffer.Release(handle->m_allocation);

			MemoryAllocation alloc = batch.BatchBuffer.Allocate(static_cast<int>(handle->m_mesh->Vertices.size()));
			if (!alloc.Allocated())
				rebuildBatchBuffer = true;
			else
				handle->m_allocation = alloc;
		}
		if (handle->m_stagedForRemoval)
		{
			batch.BatchBuffer.Release(handle->m_allocation);
			toRemove.push_back(index);
			rebuildIndexBuffer = true;
		}
		++index;
	}

	if (batch.Dynamic)
	{
		d_batch->TransformBuffer.CopyData(transformMats, 0);
		if (d_batch->HasParameters)
			d_batch->ParameterBuffer.CopyData(perObjectParams, 0);
	}

	std::sort(toRemove.begin(), toRemove.end(), std::greater<>());
	for (int index : toRemove)
	{
		if (batch.Dynamic)
			d_batch->FreeIds.push_back(batch.EntityHandles[index]->m_objectId);
		delete batch.EntityHandles[index];
		batch.EntityHandles[index] = batch.EntityHandles.back();
		batch.EntityHandles.pop_back();
	}

	std::vector<int> newIndices;
	if (rebuildBatchBuffer || rebuildIndexBuffer)
	{
		if (rebuildBatchBuffer)
		{
			batch.BatchBuffer.Destroy();
			batch.BatchBuffer = SubmitBuffer<RenderVertex>(static_cast<int>(vertices * 1.2f));
			batch.AttributeArray.SetBuffer(0, batch.BatchBuffer.GetBuffer());
			if (batch.Dynamic)
			{
				d_batch->ObjectIdBuffer = Buffer<int>(batch.BatchBuffer.GetBuffer().GetLength(), BufferOptions(false, false, false, false, true, false));
				batch.AttributeArray.SetBuffer(1, d_batch->ObjectIdBuffer);
			}
		}
		for (EntityRenderHandle *handle : batch.EntityHandles)
		{
			if (rebuildBatchBuffer)
				handle->m_allocation = batch.BatchBuffer.Allocate(static_cast<int>(handle->m_mesh->Vertices.size()));
			if (handle->m_rebuildData)
			{
				std::vector<RenderVertex> tData(handle->m_mesh->Vertices);
				std::vector<int> idData;
				if (batch.Dynamic)
				{
					for (RenderVertex& vert : tData)
						idData.push_back(handle->m_objectId);
					static_cast<DynamicBatch&>(batch).ObjectIdBuffer.CopyData(idData, handle->m_allocation.GetPosition());
				}
				else
					transformVertices(tData, handle->m_transform);
				batch.BatchBuffer.UploadData(handle->m_allocation, tData.data(), 0, static_cast<int>(tData.size()));
			}
			for (int index : handle->m_mesh->Indices)
				newIndices.push_back(index + handle->m_allocation.GetPosition());
			handle->m_rebuildData = false;
		}
		batch.IndexBuffer.Destroy();
		batch.IndexBuffer = Buffer<int>(newIndices, BufferOptions(false, false, false, false, true, false));
		batch.AttributeArray.SetIndexBuffer(batch.IndexBuffer);
	}
}

void MeshRenderer::transformVertices(std::vector<RenderVertex>& data, glm::mat4 transform)
{
	glm::mat3 normalMat = glm::inverse(glm::transpose(glm::mat3(transform)));
	for (RenderVertex& vertex : data)
	{
		vertex.Position = transform * glm::vec4(vertex.Position, 1);
		vertex.Normal = normalMat * vertex.Normal;
		vertex.Tangent = normalMat * vertex.Tangent;
	}
}
