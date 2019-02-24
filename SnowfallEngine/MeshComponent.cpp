#include "MeshComponent.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "Snowfall.h"

std::vector<SerializationField> ComponentDescriptor<MeshRenderComponent>::GetSerializationFields()
{
	return {
	};
}

MeshRenderingSystem::~MeshRenderingSystem()
{
	for (BatchState *state : m_staticBatches)
	{
		delete state;
	}
	for (BatchState *state : m_dynamicBatches)
	{
		delete state;
	}
	for (BatchState *state : m_instancedBatches)
	{
		delete state;
	}
}

void MeshRenderingSystem::InitializeSystem(Scene& scene)
{
	m_scene = &scene;
}

void transformVertices(std::vector<RenderVertex>& data, glm::mat4 transform)
{
	glm::mat3 normalMat = glm::inverse(glm::transpose(glm::mat3(transform)));
	for (RenderVertex& vertex : data)
	{
		vertex.Position = transform * glm::vec4(vertex.Position, 1);
		vertex.Normal = normalMat * vertex.Normal;
		vertex.Tangent = normalMat * vertex.Tangent;
	}
}

void MeshRenderingSystem::Update(float deltaTime)
{
	for (MeshRenderComponent *render : m_scene->GetComponentManager().GetComponents<MeshRenderComponent>())
	{
		TransformComponent *transform = render->Owner.GetComponent<TransformComponent>();
		if (!render->Batch)
			BatchNewObject(*render, *transform);
	}
	for (MeshRenderComponent *render : m_scene->GetComponentManager().GetDeadComponents<MeshRenderComponent>())
	{
		if (render->Batch) 
			render->Batch->Handles[render->MemberIndex].Remove = true;
	}
	for (BatchState *state : m_staticBatches)
	{
		CheckForRemoval(*state); //TODO: Remove batch if empty
		UploadRenderingCommands(*state);
	}
	for (BatchState *state : m_dynamicBatches)
	{
		CheckForRemoval(*state);
		UpdateDynamicBuffers(*state);
		UploadRenderingCommands(*state);
	}
	for (BatchState *state : m_instancedBatches)
	{
		CheckForRemoval(*state);
		UpdateDynamicBuffers(*state);
		UploadRenderingCommands(*state);
	}
}

std::string MeshRenderingSystem::GetName()
{
	return "MeshRenderingSystem";
}

std::vector<std::string> MeshRenderingSystem::GetSystemsBefore()
{
	return { "TransformSystem" };
}

std::vector<std::string> MeshRenderingSystem::GetSystemsAfter()
{
	return { "CameraSystem" };
}

void MeshRenderingSystem::BatchNewObject(MeshRenderComponent& mcomp, TransformComponent& tcomp)
{
	bool foundBatch = false;
	switch (mcomp.BatchingType)
	{
	case BatchingType::Static:
		for (BatchState *statePtr : m_staticBatches)
		{
			BatchState& state = *statePtr;
			if (mcomp.SoleBatch)
				break;
			if (!state.SoleBatch && state.LayerMask == mcomp.LayerMask && state.Material == mcomp.Material)
			{
				foundBatch = true;
				BatchStatic(state, mcomp, tcomp);
				break;
			}
		}
		if (!foundBatch)
		{
			m_staticBatches.push_back(new BatchState);
			BatchStatic(*m_staticBatches.back(), mcomp, tcomp);
		}
		break;
	case BatchingType::Dynamic:
		for (BatchState *statePtr : m_dynamicBatches)
		{
			BatchState& state = *statePtr;
			if (mcomp.SoleBatch)
				break;
			if (!state.SoleBatch && state.LayerMask == mcomp.LayerMask && state.Material == mcomp.Material)
			{
				foundBatch = true;
				BatchDynamic(state, mcomp, tcomp, false);
				break;
			}
		}
		if (!foundBatch)
		{
			m_dynamicBatches.push_back(new BatchState);
			BatchDynamic(*m_dynamicBatches.back(), mcomp, tcomp, false);
		}
		break;
	case BatchingType::Instanced:
		for (BatchState *statePtr : m_instancedBatches)
		{
			BatchState& state = *statePtr;
			if (mcomp.SoleBatch)
				break;
			if (!state.SoleBatch && state.LayerMask == mcomp.LayerMask && state.Material == mcomp.Material && state.Mesh == mcomp.Mesh)
			{
				foundBatch = true;
				BatchDynamic(state, mcomp, tcomp, true);
				break;
			}
		}
		if (!foundBatch)
		{
			m_instancedBatches.push_back(new BatchState);
			BatchDynamic(*m_instancedBatches.back(), mcomp, tcomp, true);
		}
		break;
	}
}

void MeshRenderingSystem::BatchStatic(BatchState& state, MeshRenderComponent& mcomp, TransformComponent& tcomp)
{
	Mesh& mesh = mcomp.Mesh->GetMesh();
	GeometryHandle handle = Snowfall::GetGameInstance().GetMeshManager().CreateGeometry(mesh.Vertices.size(), mesh.Indices.size());
	state.Handles.push_back(BatchMember());
	state.BatchType = BatchingType::Static;
	state.LayerMask = mcomp.LayerMask;
	state.SoleBatch = mcomp.SoleBatch;
	state.Material = mcomp.Material;

	state.StateChange.LayerMask = state.LayerMask;
	state.ShaderSpecialization = { "STATIC" };
	state.StateChange.Type = PrimitiveType::Triangles;
	state.StateChange.InstanceCount = 1;

	BatchMember& member = state.Handles.back();
	member.Geometry = handle;
	member.Component = &mcomp;
	mcomp.MemberIndex = static_cast<int>(state.Handles.size()) - 1;
	mcomp.Batch = &state;
	
	std::vector<RenderVertex> vertices(mesh.Vertices);
	transformVertices(vertices, tcomp.ModelMatrix);

	Snowfall::GetGameInstance().GetMeshManager().WriteGeometryVertices(handle, vertices.data(), 0, vertices.size());
	Snowfall::GetGameInstance().GetMeshManager().WriteGeometryIndices(handle, mesh.Indices.data(), 0, mesh.Indices.size());
}

void MeshRenderingSystem::BatchDynamic(BatchState& state, MeshRenderComponent& mcomp, TransformComponent& tcomp, bool instanced)
{
	GeometryHandle handle;
	auto masset = mcomp.Mesh;
	Mesh& mesh = masset->GetMesh();
	if (!instanced || !state.Mesh)
	{
		state.StateChange.InstanceCount = 1;
		state.Mesh = masset;
		state.Geometry = masset->GetGeometry();
	}
	state.Handles.push_back(BatchMember());
	state.BatchType = instanced ? BatchingType::Instanced : BatchingType::Dynamic;
	state.LayerMask = mcomp.LayerMask;
	state.SoleBatch = mcomp.SoleBatch;
	state.Material = mcomp.Material;

	state.StateChange.LayerMask = state.LayerMask;
	state.ShaderSpecialization = { instanced ? "INSTANCED" : "DYNAMIC" };
	state.StateChange.Type = PrimitiveType::Triangles;
	
	if (instanced)
		++state.StateChange.InstanceCount;

	BatchMember& member = state.Handles.back();
	member.Geometry = handle;
	member.Transform = &tcomp;
	member.Component = &mcomp;
	mcomp.MemberIndex = state.Handles.size() - 1;
	mcomp.Batch = &state;
}

bool MeshRenderingSystem::CheckForRemoval(BatchState& state)
{
	int index = 0;
	std::vector<int> toRemove;
	for (BatchMember& member : state.Handles)
	{
		if (member.Remove)
		{
			if (state.StateChange.InstanceCount > 1)
				--state.StateChange.InstanceCount;
			toRemove.push_back(member.Component->MemberIndex); //Investigate Shadow Meshes
			continue;
		}
		member.Component->MemberIndex = index;
		++index;
	}
	std::sort(toRemove.begin(), toRemove.end(), std::greater<>());
	for (int i : toRemove)
		state.Handles.erase(state.Handles.begin() + i);
	return state.Handles.size() == 0;
}

void MeshRenderingSystem::UpdateDynamicBuffers(BatchState& state)
{
	Material& material = state.Material->GetMaterial();
	std::vector<glm::mat4> matrices;
	std::vector<glm::vec4> params(material.PerObjectParameterCount * state.Handles.size());
	std::vector<int> toRemove;
	int index = 0;
	for (BatchMember& member : state.Handles)
	{
		member.Component->MemberIndex = index;
		matrices.push_back(member.Transform->ModelMatrix);
		matrices.push_back(glm::mat4(glm::inverse(glm::transpose(glm::mat3(member.Transform->ModelMatrix)))));
		params.insert(params.begin() + index * material.PerObjectParameterCount, member.Component->ObjectParameters.begin(),
			member.Component->ObjectParameters.begin() + std::min(material.PerObjectParameterCount, static_cast<int>(member.Component->ObjectParameters.size()))); // Account for bad parameter counts
		++index;
	}
	if (matrices.size() > state.TransformBuffer.GetLength())
	{
		state.TransformBuffer.Destroy();
		state.ObjectParameterBuffer.Destroy();
		state.TransformBuffer = Buffer<glm::mat4>(matrices.size() * sizeof(glm::mat4), BufferOptions(false, false, false, false, true, false));
		state.ObjectParameterBuffer = Buffer<glm::vec4>(params.size() * sizeof(glm::vec4), BufferOptions(false, false, false, false, true, false));
	}
	state.StateChange.Descriptor = ShaderDescriptor();

	state.TransformBuffer.CopyData(matrices, 0);
	state.StateChange.Descriptor.AddShaderStorageBuffer(state.TransformBuffer, 0);

	if (params.size() != 0)
	{
		state.StateChange.Descriptor.AddShaderStorageBuffer(state.ObjectParameterBuffer, 1);
		state.ObjectParameterBuffer.CopyData(params, 0);
	}
}

void MeshRenderingSystem::UploadRenderingCommands(BatchState& state)
{
	std::vector<GeometryHandle> ghandles;
	std::vector<BoundingBox> boxes;
	Material& material = state.Material->GetMaterial();
	state.StateChange.Shader = material.MaterialShader;
	state.StateChange.Specializations = state.ShaderSpecialization;
	state.StateChange.Constants = material.Constants;
	if (state.StateChange.InstanceCount > 1)
	{
		state.StateChange.Constants.AddConstant(2, material.PerObjectParameterCount);
		state.StateChange.Constants.AddConstant(3, 0);
		ghandles.push_back(state.Geometry);
		boxes.push_back(BoundingBox());
	}
	else
	{
		for (BatchMember& member : state.Handles)
		{
			ghandles.push_back(member.Geometry);
			boxes.push_back(BoundingBox());
		}
	}
	Snowfall::GetGameInstance().GetMeshManager().WriteIndirectCommandsToCullingPass(ghandles, boxes, state.StateChange);
} 
