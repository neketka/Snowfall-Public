#include "Batching.h"
#include "Scene.h"

void StaticBatcher::AddEntity(EntityData& data)
{
	EntityBatchParams params;
	params.Entity = data.Entity;

	StaticBatchParams bparams;
	data.Entity->OnStaticBatch(bparams);
	std::vector<RenderVertex> transformed;

	glm::mat3 normalMat = glm::mat3(glm::transpose(glm::inverse(bparams.TransformMatrix)));
	for (RenderVertex v : bparams.Mesh.Vertices)
	{
		RenderVertex nw;
		nw.Color = v.Color;
		nw.Normal = normalMat * v.Normal;
		nw.Position = glm::vec3(bparams.TransformMatrix * glm::vec4(v.Position, 1));
		nw.Tangent = v.Tangent;
		nw.Texcoord = v.Texcoord;
		transformed.push_back(nw);
	}

	StaticBatch& batch = StaticBatch();
	bool foundBatch = false;
	if (!data.BatchOptions.OwnBatch)
	{
		for (StaticBatch& fbatch : m_batches)
		{
			if (!(fbatch.Closed && fbatch.Material == data.BatchOptions.Material))
			{
				batch = fbatch;
				foundBatch = true;
				break;
			}
		}
	}
	else
	{
		batch.Closed = true;
	}

	if (!foundBatch)
	{
		params.VertexOffset = 0;
		params.VertexLength = bparams.Mesh.Vertices.size();
		params.IndexOffset = 0;
		params.IndexLength = bparams.Mesh.Indices.size();

		batch.Material = data.BatchOptions.Material;
		batch.Vertices = Buffer<RenderVertex>(transformed, false, false, false);
		batch.Indices = Buffer<int>(bparams.Mesh.Indices, false, false, false);
		batch.Attributes = VertexArray(
			{
				BufferStructure(
				{
					Attribute(0, 0, 3, 12, true), //Position
					Attribute(1, 12, 4, 16, true), //Color
					Attribute(2, 28, 3, 12, true), //Normal
					Attribute(3, 40, 3, 12, true), //Tangent
					Attribute(4, 52, 2, 8, true) //Texcoord
				})
			});
		batch.Attributes.SetBuffer(0, batch.Vertices);
		batch.Attributes.SetIndexBuffer(batch.Indices);
	}
	else
	{
		params.VertexOffset = batch.Vertices.GetLength();
		params.VertexLength = bparams.Mesh.Vertices.size();
		params.IndexOffset = batch.Indices.GetLength();
		params.IndexLength = bparams.Mesh.Indices.size();

		Buffer<RenderVertex> verts(batch.Vertices.GetLength() + bparams.Mesh.Vertices.size(), false, false, false);
		Buffer<int> inds(batch.Indices.GetLength() + bparams.Mesh.Indices.size(), false, false, false);

		batch.Vertices.CopyBufferData(0, batch.Vertices.GetLength(), verts, 0);
		batch.Indices.CopyBufferData(0, batch.Indices.GetLength(), inds, 0);
		batch.Vertices.CopyData(bparams.Mesh.Vertices, params.VertexOffset);
		batch.Indices.CopyData(bparams.Mesh.Indices, params.IndexOffset);

		batch.Vertices.Destroy();
		batch.Indices.Destroy();

		batch.Vertices = verts;
		batch.Indices = inds;

		batch.Attributes.SetBuffer(0, batch.Vertices);
		batch.Attributes.SetIndexBuffer(batch.Indices);
	}

	if (!foundBatch)
	{
		m_batches.push_back(batch);
		params.Batch = &m_batches.data()[m_batches.size() - 1];
	}
	else
		params.Batch = &batch;

	m_entities.insert(std::make_pair(data.Entity->GetUUID(), params));
}

void StaticBatcher::RemoveEntity(EntityUUID uuid)
{
	EntityBatchParams params = m_entities.at(uuid);
	StaticBatch& batch = *static_cast<StaticBatch *>(params.Batch);

	Buffer<RenderVertex> newVert = Buffer<RenderVertex>(batch.Vertices.GetLength() - params.VertexLength, false, false, false);
	Buffer<int> newInd = Buffer<int>(batch.Indices.GetLength() - params.IndexLength, false, false, false);

	batch.Vertices.CopyBufferData(0, params.VertexOffset, newVert, 0);
	batch.Vertices.CopyBufferData(params.VertexOffset + params.VertexLength,
		batch.Vertices.GetLength() - params.VertexOffset - params.VertexLength, newVert, params.VertexOffset);

	batch.Indices.CopyBufferData(0, params.IndexOffset, newInd, 0);
	batch.Indices.CopyBufferData(params.IndexOffset + params.IndexLength,
		batch.Indices.GetLength() - params.IndexOffset - params.IndexLength, newInd, params.IndexOffset);

	m_entities.erase(uuid);
}

void StaticBatcher::Render(CommandBuffer& cmdBuff, ICamera& camera)
{
	for (StaticBatch& batch : m_batches)
	{
		Pipeline pipeline;
		ShaderConstants constants = batch.Material.Constants;
		ShaderDescriptor descriptor;
		
		pipeline.Shader = batch.Material.MaterialShader;
		pipeline.VertexStage.VertexArray = batch.Attributes;
		pipeline.FragmentStage.DepthTest = true;
		pipeline.FragmentStage.Framebuffer = camera.GetRenderTarget();
		pipeline.FragmentStage.Viewport = camera.GetRegion();
		pipeline.FragmentStage.DrawTargets = { 0 };

		constants.AddConstant(0, camera.GetViewMatrix());
		constants.AddConstant(1, camera.GetProjectionMatrix());

		cmdBuff.BindPipelineCommand(pipeline);
		cmdBuff.BindConstantsCommand(constants);
		cmdBuff.BindDescriptorCommand(descriptor);
		cmdBuff.DrawIndexedCommand(PrimitiveType::Triangles, batch.Indices.GetLength(), 1, 0, 0);
	}
}