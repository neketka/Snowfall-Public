#include "Scene.h"
#include "Snowfall.h"

Scene::Scene()
{
	m_evManager = new EventManager;
	m_cManager = new ComponentManager(&Snowfall::GetGameInstance().GetPrototypeManager());
	m_eManager = new EntityManager(&Snowfall::GetGameInstance().GetPrototypeManager(), m_cManager);
	m_sManager = new SystemManager(m_eManager, m_cManager, m_evManager, &Snowfall::GetGameInstance().GetPrototypeManager());
}

Scene::~Scene()
{
	delete m_evManager;
	delete m_cManager;
	delete m_eManager;
	delete m_sManager;
}

void Scene::Update(float deltaTime)
{
	m_sManager->UpdateSystems(deltaTime);
	m_cManager->CleanDeadComponents();
}
