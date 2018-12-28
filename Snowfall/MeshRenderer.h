#pragma once
#include "SubmitBuffer.h"
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"
#include "CommandBuffer.h"
#include "Transformable.h"

#include <vector>

class EntityRenderHandle
{
public:
	EntityRenderHandle(EntityOptions options);
	void SetMesh(Mesh& mesh);
	void SetMaterial(Material& material);
	void SetTransform(glm::mat4 transform);
	void Delete();
private:
	Mesh *m_mesh;
	Material *m_material;
	int m_objectId;
	bool m_rebuildData;
	bool m_stagedForRemoval;
	glm::mat4 m_transform;
	MemoryAllocation m_allocation;
	EntityOptions m_options;
	friend class MeshRenderer;
};

class Batch
{
public:
	Batch() : BatchBuffer(0) {}
	SubmitBuffer<RenderVertex> BatchBuffer;
	Buffer<int> IndexBuffer;
	VertexArray AttributeArray;
	std::vector<EntityRenderHandle *> EntityHandles;
	Material *Material;

	bool Dynamic;
	bool Instanced;
};

class StaticBatch : public Batch
{
public:
	StaticBatch() 
	{
		Dynamic = false;
		Instanced = false;
	}
};

const int MaxEntitiesPerDynamicBatch = 128;
class DynamicBatch : public Batch
{
public:
	Buffer<glm::mat4> TransformBuffer;
	std::vector<int> FreeIds;
	DynamicBatch()
	{
		Dynamic = true;
		Instanced = false;
	}
};

class MeshRenderer
{
public:
	MeshRenderer();
	~MeshRenderer();

	void UpdateEntities();
	void Render(ICamera& camera, CommandBuffer& buffer);
	EntityRenderHandle& CreateRenderEntity(EntityOptions options);
private:
	VertexArray createVertexFormat(bool instanced);
	void scanUnbatched();
	void renderBatch(Batch& batch, ICamera& camera, CommandBuffer& cmdBuffer);
	void updateBatch(Batch& batch);
	void transformVertices(std::vector<RenderVertex>& data, glm::mat4 transform);

	std::vector<EntityRenderHandle *> m_unbatched;

	std::vector<StaticBatch> m_staticBatches;
	std::vector<DynamicBatch> m_dynamicBatches;
};

