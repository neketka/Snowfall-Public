#pragma once
#include "Texture.h"
#include "Material.h"
#include "Mesh.h"
#include "Quad.h"
#include "Framebuffer.h"
#include "BoundingBox.h"
#include "Transformable.h"
#include "CommandBuffer.h"
#include "CollisionShape.h"
#include "MeshRenderer.h"

#include <glm/glm.hpp>

using EntityUUID = int;

class Scene;

#define DEFINE_PROTOTYPE(EntityTypename) \
virtual Entity *CreateInstance() override { return dynamic_cast<Entity *>(new EntityTypename); } \
virtual std::string GetTypeName() override { return std::string(#EntityTypename); }

class Entity : public Transformable
{
public:
	Entity(std::string name, EntityOptions entityOptions);
	inline EntityUUID GetUUID() { return m_uuid; } // Get the Unique Identifier for this entity
	inline EntityOptions GetEntityOptions() { return m_options; }
	inline Scene& GetScene() { return *m_scene; }

	inline void SetName(std::string name) { m_name = name; }
	inline std::string GetName() { return m_name; }

	void Update(float deltaTime);
	void CustomRender(ICamera& camera, CommandBuffer& buffer);
	void PostPhysicsUpdate();
	void RenderUI(ICamera& camera);
	void AddToScene(Scene& scene);

	virtual Entity *CreateInstance() = 0;
	virtual std::string GetTypeName() = 0;

	void Destroy();

	void RegisterRenderer(EntityRenderHandle *handle);
	void SetScene(Scene& scene);
	
protected:
	void SetMesh(Mesh& mesh);
	void SetMaterial(Material& material);
	void SetCollisionShape(CollisionShape& shape);
	void SetInstances(int instances);
	void SetMaterialParameter(int index, glm::vec4 param);
	void SetLayerMask(LayerMask mask);

	virtual void OnSceneAddition() = 0; // Called after scene addition
	virtual void OnUpdate(float deltaTime) {} // Called once per frame 
	virtual void OnUIRender(ICamera& camera) {} // Called once per UI camera after rendering
	virtual void OnRenderPass(ICamera& camera, CommandBuffer& buffer) {}

private:
	Scene *m_scene;
	EntityUUID m_uuid;
	EntityOptions m_options;
	EntityRenderHandle *m_renderHandle;
	std::string m_name;
};