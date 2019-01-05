#include "Entity.h"
#include "Scene.h"
#include "Snowfall.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

glm::mat4 Transformable::GetTransform()
{ 
	// Calculate model space transform in the order: Scale, Z X Y rotation, and translation
	glm::mat4 transform = glm::translate(Position) * glm::rotate(Rotation.y, glm::vec3(0, 1, 0)) *
		glm::rotate(Rotation.x, glm::vec3(1, 0, 0)) * glm::rotate(Rotation.z, glm::vec3(0, 0, 1)) * glm::scale(Scale);

	if (m_parent)
		return m_parent->GetTransform() * transform;

	return transform;
}

void Transformable::SetParent(Transformable& transformable)
{
	m_parent = &transformable;
	transformable.m_children.push_back(this);
}

void Transformable::Orphan()
{
	m_parent = nullptr;
}

std::vector<Transformable*> Transformable::GetChildren()
{
	return std::vector<Transformable*>();
}

Transformable *Transformable::GetParent()
{
	return m_parent;
}

Entity::Entity(std::string name, EntityOptions options) : m_name(name), m_options(options)
{
	m_uuid = rand();
}

void Entity::Update(float deltaTime)
{
	OnUpdate(deltaTime);
}

void Entity::CustomRender(ICamera& camera, CommandBuffer& buffer)
{
	OnRenderPass(camera, buffer);
}

void Entity::PostPhysicsUpdate()
{
	if (m_renderHandle)
		m_renderHandle->SetTransform(GetTransform());
}

void Entity::RenderUI(ICamera& camera)
{
	OnUIRender(camera);
}

void Entity::AddToScene(Scene& scene)
{
	m_scene = &scene;
	OnSceneAddition();
}

void Entity::Destroy()
{
	if (m_renderHandle)
	{
		m_renderHandle->Delete();
		m_renderHandle = nullptr;
	}
	if (m_scene)
	{
		Scene& scene = *m_scene;
		m_scene = nullptr;
		scene.RemoveEntity(m_uuid);
	}
}

void Entity::RegisterRenderer(EntityRenderHandle *handle)
{
	m_renderHandle = handle;
}

void Entity::SetScene(Scene& scene)
{
	m_scene = &scene;
}

void Entity::SetMesh(Mesh& mesh)
{
	m_renderHandle->SetMesh(mesh);
}

void Entity::SetMaterial(Material& material)
{
	m_renderHandle->SetMaterial(material);
}

void Entity::SetCollisionShape(CollisionShape& shape)
{
}

void Entity::SetInstances(int instances)
{
}

void Entity::SetMaterialParameter(int index, glm::vec4 param)
{
	m_renderHandle->SetMaterialParameter(index, param);
}

void Entity::SetLayerMask(LayerMask mask)
{
	m_renderHandle->SetLayerMask(mask);
}
