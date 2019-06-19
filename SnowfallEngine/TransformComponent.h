#pragma once
#include "ECS.h"
#define GLM_FORCE_SSE42
#define GLM_FORCE_ALIGNED
#include <glm/glm.hpp>

#include "export.h"

class TransformComponent : public Component
{
public:
	TransformComponent() : Enabled(true), Scale(1, 1, 1), transformInstant(0) {}
	Entity Parent;

	glm::vec3 Position;
	glm::vec3 Rotation;
	glm::vec3 Scale;

	bool Enabled;

	glm::vec3 GlobalPosition;
	glm::vec3 GlobalRotation;
	glm::vec3 GlobalDirection;

	glm::mat4 ModelMatrix;

	long transformInstant;
};

class SNOWFALLENGINE_API TransformSystem : public ISystem
{
public:
	TransformSystem() { }
	virtual void InitializeSystem(Scene& scene) override;
	virtual void Update(float deltaTime) override;
	virtual std::string GetName() override;
	virtual std::vector<std::string> GetSystemsBefore() override;
	virtual std::vector<std::string> GetSystemsAfter() override;
	virtual bool IsMainThread() override { return true; }
private:
	Scene *m_scene = nullptr;
	long m_transformInstant = -1;
};