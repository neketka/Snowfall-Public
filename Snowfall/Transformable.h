#pragma once
#include <glm\glm.hpp>
#include <vector>

class Transformable
{
public:
	Transformable() : m_parent(nullptr), Scale(1, 1, 1) {}
	glm::mat4 GetTransform();

	void SetParent(Transformable& transformable);
	void Orphan();
	std::vector<Transformable *> GetChildren();
	Transformable *GetParent();

	glm::vec3 Position;
	glm::vec3 Rotation;
	glm::vec3 Scale;
private:
	Transformable *m_parent;
	std::vector<Transformable *> m_children;
};

class EntityOptions
{
public:
	EntityOptions() {}

	EntityOptions(EntityOptions& options)
		: Instanced(options.Instanced), Dynamic(options.Dynamic), UseCustomRendering(options.UseCustomRendering),
		EnableUpdate(options.EnableUpdate), EnablePhysics(options.EnablePhysics), EnableRendering(options.EnableRendering),
		EnableUIRendering(options.EnableUIRendering), Unbatched(options.Unbatched) {}

	EntityOptions(bool instanced, bool dynamic, bool customRendering, bool enableUpdate, 
		bool enablePhysics, bool enableRendering, bool enableUiRendering, bool unbatched)
		: Instanced(instanced), Dynamic(dynamic), UseCustomRendering(customRendering),
		EnableUpdate(enableUpdate), EnablePhysics(enablePhysics), EnableRendering(enableRendering),
		EnableUIRendering(enableUiRendering), Unbatched(unbatched) {}

	bool Instanced = false;
	bool Dynamic = true;
	bool Skeletal = false;
	bool Unbatched = false;

	bool UseCustomRendering = false;

	bool EnableUpdate = true;
	bool EnablePhysics = true;
	bool EnableRendering = true;
	bool EnableUIRendering = false;
};