#pragma once
#include "ECS.h"
#include "Material.h"
#include "MeshManager.h"
#include "MaterialAsset.h"
#include "MeshAsset.h"
#include "TransformComponent.h"

enum class BatchingType
{
	Static, Dynamic, Instanced
};

class BatchState;

class MeshRenderComponent : public Component
{
public:
	MeshRenderComponent() : BatchingType(BatchingType::Dynamic), LayerMask(0xFFFFFFFFFFFFFFFF) {}
	MaterialAsset *Material;
	MeshAsset *Mesh;
	LayerMask LayerMask;
	std::vector<glm::vec4> ObjectParameters;
	BatchingType BatchingType;
	bool SoleBatch;

	BatchState *Batch;
	int MemberIndex;
};

class BatchMember
{
public:
	TransformComponent *Transform;
	MeshRenderComponent *Component;

	GeometryHandle Geometry;
	bool Remove;
};

class BatchState
{
public:
	RendererStateChange StateChange;
	MaterialAsset *Material;
	LayerMask LayerMask;
	std::vector<BatchMember> Handles;
	BatchingType BatchType;
	Buffer<glm::mat4> TransformBuffer;
	Buffer<glm::vec4> ObjectParameterBuffer;
	std::set<std::string> ShaderSpecialization;
	MeshAsset *Mesh;
	GeometryHandle Geometry;
	bool SoleBatch;
};

class MeshRenderingSystem : public ISystem
{
public:
	~MeshRenderingSystem();
	virtual void InitializeSystem(EntityManager *eManager, ComponentManager *cManager, EventManager *evManager, PrototypeManager *pManager) override;
	virtual void Update(float deltaTime) override;
	virtual std::string GetName() override;
	virtual std::vector<std::string> GetSystemsBefore() override;
	virtual std::vector<std::string> GetSystemsAfter() override;
	virtual bool IsMainThread() override { return true; }
private:
	void BatchNewObject(MeshRenderComponent& mcomp, TransformComponent& tcomp);
	void BatchStatic(BatchState& state, MeshRenderComponent& mcomp, TransformComponent& tcomp);
	void BatchDynamic(BatchState& state, MeshRenderComponent& mcomp, TransformComponent& tcomp, bool instanced);
	bool CheckForRemoval(BatchState& state);
	void UpdateDynamicBuffers(BatchState& state);
	void UploadRenderingCommands(BatchState& state);

	std::vector<BatchState *> m_staticBatches;
	std::vector<BatchState *> m_dynamicBatches;
	std::vector<BatchState *> m_instancedBatches;
	EntityManager *m_eManager;
	ComponentManager *m_cManager;
};