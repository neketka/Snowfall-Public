#pragma once
#include "ECS.h"

class PhysicsConstraintSystem : public ISystem
{
public:
	virtual void InitializeSystem(Scene& scene) override;
	virtual void Update(float deltaTime) override;
	virtual std::string GetName() override;
	virtual std::vector<std::string> GetSystemsBefore() override;
	virtual std::vector<std::string> GetSystemsAfter() override;
	virtual bool IsMainThread() override;
};

class PhysicsPointConstraintComponent : public Component
{
public:
	Entity ObjectA;
	glm::vec3 PivotA;

	bool UsingObjectB;
	Entity ObjectB;
	glm::vec3 PivotB;
};

class PhysicsHingeConstraintComponent : public Component 
{

};

class PhysicsSliderConstraintComponent : public Component
{

};

class PhysicsConeConstraintComponent : public Component
{

};

class PhysicsSpringConstraintComponent : public Component
{

};