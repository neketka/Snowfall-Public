#pragma once
#include "Entity.h"
#include "VertexArray.h"
#include "Buffer.h"
#include "MeshRenderer.h"
#include "SceneConfiguration.h"

#include <memory>
#include <map>
#include <string>

class EntityData
{
public:
	Entity *Entity;
};

class Scene
{
public:
	Scene(SceneConfiguration *config);
	~Scene();
	template<class T>
	T& AddEntity()
	{
		return reinterpret_cast<T&>(*AddEntity(dynamic_cast<Entity *>(new T)));
	}
	void RemoveEntity(EntityUUID uuid);
	Entity& AddEntity(std::string typeName);

	EntityUUID GetEntityByName(std::string name);
	Entity& GetEntityObject(EntityUUID uuid);

	void AddCamera(ICamera& camera);
	void RemoveCamera(ICamera& camera);

	void PerformUpdate(float deltaTime);
	void RenderUI(ICamera& camera);
	void RenderScene(float deltaTime, ICamera& camera);

	void RenderCameras(float deltaTime);

	inline MeshRenderer& GetMeshRenderer() { return m_meshRenderer; }
private:
	Entity *AddEntity(Entity *entity);
	MeshRenderer m_meshRenderer;

	SceneConfiguration *m_sceneConfiguration;

	std::map<int, EntityData> m_entities;
	std::vector<ICamera *> m_cameras;

	std::vector<EntityUUID> m_logicEntities;
	std::vector<EntityUUID> m_physicsEntities;
	std::vector<EntityUUID> m_proceduralEntities;
	std::vector<EntityUUID> m_uiEntities;
	std::vector<EntityUUID> m_renderEntities;
};

