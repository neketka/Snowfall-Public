#include "stdafx.h"
#include "PhysicsConstraintSystem.h"

void PhysicsConstraintSystem::InitializeSystem(Scene& scene)
{
}

void PhysicsConstraintSystem::Update(float deltaTime)
{
}

std::string PhysicsConstraintSystem::GetName()
{
	return "PhysicsConstraintSystem";
}

std::vector<std::string> PhysicsConstraintSystem::GetSystemsBefore()
{
	return { };
}

std::vector<std::string> PhysicsConstraintSystem::GetSystemsAfter()
{
	return { };
}

bool PhysicsConstraintSystem::IsMainThread()
{
	return false;
}
