#include "Scene.h"

Scene::Scene(SceneConfiguration *config) : m_sceneConfiguration(config)
{
}

Scene::~Scene()
{
	while (m_entities.size() > 0)
	{
		auto entity = *m_entities.begin();
		entity.second.Entity->Destroy();
	}
	delete m_sceneConfiguration;
}

Entity *Scene::AddEntity(Entity *entity)
{
	EntityData data; // Entity specific data
	data.Entity = entity;
	EntityOptions options = entity->GetEntityOptions(); // Get entity data

	if (options.EnableRendering)
		entity->RegisterRenderer(&m_meshRenderer.CreateRenderEntity(options));
	entity->AddToScene(*this); 

	int uuid = entity->GetUUID();
	m_entities.insert(std::make_pair(uuid, data)); // Put entity into global list

	// Put object into specific lists
	if (options.EnableUpdate)
		m_logicEntities.push_back(uuid);
	if (options.EnablePhysics)
		m_physicsEntities.push_back(uuid);
	if (options.EnableUIRendering)
		m_uiEntities.push_back(uuid);
	if (options.UseCustomRendering)
		m_proceduralEntities.push_back(uuid);
	if (options.EnableRendering)
		m_renderEntities.push_back(uuid);
	return entity;
}

void Scene::RemoveEntity(EntityUUID uuid)
{
	m_entities[uuid].Entity->Destroy();
	
	auto logicIter = std::find(m_logicEntities.begin(), m_logicEntities.end(), uuid);
	if (logicIter != m_logicEntities.end())
		m_logicEntities.erase(logicIter);

	auto physicsIter = std::find(m_physicsEntities.begin(), m_physicsEntities.end(), uuid);
	if (physicsIter != m_physicsEntities.end())
		m_physicsEntities.erase(physicsIter);

	auto uiIter = std::find(m_uiEntities.begin(), m_uiEntities.end(), uuid);
	if (uiIter != m_uiEntities.end())
		m_uiEntities.erase(uiIter);

	auto proceduralIter = std::find(m_proceduralEntities.begin(), m_proceduralEntities.end(), uuid);
	if (proceduralIter != m_proceduralEntities.end())
		m_proceduralEntities.erase(proceduralIter);

	auto renderIter = std::find(m_renderEntities.begin(), m_renderEntities.end(), uuid);
	if (renderIter != m_renderEntities.end())
		m_renderEntities.erase(renderIter);

	Entity *entity = m_entities[uuid].Entity;
	delete entity;

	m_entities.erase(uuid);
}

Entity& Scene::AddEntity(std::string typeName)
{
	return *AddEntity(m_sceneConfiguration->GetEntityPrototype(typeName)->CreateInstance());
}

EntityUUID Scene::GetEntityByName(std::string name)
{
	for (auto entity : m_entities)
		return entity.first;
	return -1;
}

Entity& Scene::GetEntityObject(EntityUUID uuid)
{
	return *m_entities.at(uuid).Entity;
}

void Scene::SetMainCamera(ICamera & camera)
{
}

void Scene::AddCamera(ICamera& camera)
{
	m_cameras.push_back(&camera);
}

void Scene::RemoveCamera(ICamera& camera)
{
	m_cameras.erase(std::find(m_cameras.begin(), m_cameras.end(), &camera));
}

void Scene::PerformUpdate(float deltaTime)
{
	for (EntityUUID uuid : m_logicEntities)
	{
		EntityData e = m_entities[uuid];
		e.Entity->Update(deltaTime);
	}

	for (EntityUUID uuid : m_physicsEntities)
	{
		EntityData e = m_entities[uuid];
		// TODO: perform physics transform update here
	}

	for (auto entity : m_entities)
		entity.second.Entity->PostPhysicsUpdate();

	m_meshRenderer.UpdateEntities();
}

void Scene::RenderUI(ICamera& camera)
{
	for (EntityUUID uuid : m_uiEntities)
	{
		EntityData e = m_entities[uuid];
		e.Entity->RenderUI(camera);
	}
}

void Scene::RenderScene(ICamera& camera)
{
	CommandBuffer buffer;
	camera.GetRenderTarget().ClearColor(0, glm::vec4(0, 0, 0, 1));
	camera.GetRenderTarget().ClearDepth(1);

	for (EntityUUID uuid : m_proceduralEntities)
	{ 
		EntityData e = m_entities[uuid];
		e.Entity->CustomRender(camera, buffer);
	}
	m_meshRenderer.Render(camera, buffer);
	buffer.ExecuteCommands();
}

void Scene::RenderCameras(float deltaTime)
{
	for (ICamera *camera : m_cameras)
	{
		this->RenderScene(*camera);
		if (camera->HasUI())
			this->RenderUI(*camera);
	}
}
