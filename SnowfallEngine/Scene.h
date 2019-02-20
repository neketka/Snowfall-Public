#pragma once
#include "ECS.h"

#include "export.h"

class SNOWFALLENGINE_API Scene
{
public:
	Scene();
	~Scene();

	inline EntityManager& GetEntityManager() { return *m_eManager; }
	inline ComponentManager& GetComponentManager() { return *m_cManager; }
	inline SystemManager& GetSystemManager() { return *m_sManager; }
	inline EventManager& GetEventManager() { return *m_evManager; }

	void Update(float deltaTime);
private:
	EntityManager *m_eManager;
	ComponentManager *m_cManager;
	SystemManager *m_sManager;
	EventManager *m_evManager;
};
