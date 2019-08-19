#include "stdafx.h"
#include "ParticleGenerationSystem.h"

void ParticleGenerationSystem::InitializeSystem(Scene& scene)
{
	m_scene = &scene;
}

void ParticleGenerationSystem::Update(float deltaTime)
{
	for (ParticleSystemComponent *comp : m_scene->GetComponentManager().GetComponents<ParticleSystemComponent>())
	{

	}
}

std::string ParticleGenerationSystem::GetName()
{
	return "ParticleGenerationSystem";
}

std::vector<std::string> ParticleGenerationSystem::GetSystemsBefore()
{
	return std::vector<std::string>();
}

std::vector<std::string> ParticleGenerationSystem::GetSystemsAfter()
{
	return std::vector<std::string>();
}

bool ParticleGenerationSystem::IsMainThread()
{
	return false;
}
