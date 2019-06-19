#include "module.h"

#include "CameraFlyComponent.h"
#include "TestComponent.h"

void SnowfallGame::InitializePrototypes(PrototypeManager& manager)
{
	manager.AddComponentDescription<CameraFlyComponent>();
	manager.AddComponentDescription<TestComponent>();

	manager.AddSystemPrototype<CameraFlySystem>();
	manager.AddSystemPrototype<TestSystem>();
}

std::string SnowfallGame::GetModuleName()
{
	return "SnowfallGame";
}

Module *CreateModule()
{
	return new SnowfallGame;
}