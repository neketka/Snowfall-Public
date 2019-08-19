#pragma once
#include "ECS.h"
#include "BoundingBox.h"
#include "MaterialAsset.h"

class ParticleStruct
{
public:
	glm::vec4 Position; // + Life
	glm::vec4 Velocity; // + Gravity Factor
	glm::vec4 Scale; // + Time passed
	glm::vec4 Color;
};

class ParticleFadeProperty
{
public:
	float MinTime;
	float MaxTime;
	float MinFactor;
	float MaxFactor;
};

class ParticleSystemComponent : public Component
{
public:
	MaterialAsset *BillboardTexture;

	glm::vec4 ColorMin;
	glm::vec4 ColorMax;

	float GravityFactorMin;
	float GravityFactorMax;

	glm::vec3 EmissionDirection;
	float EmissionConeAngle;

	float EmissionVelocityMin;
	float EmissionVelocityMax;

	BoundingBox SpawnArea;
	BoundingBox KillBox;

	float LifeMin;
	float LifeMax;

	glm::vec3 ScaleMin;
	glm::vec3 ScaleMax;

	ParticleFadeProperty ColorLifeFade;
	ParticleFadeProperty VelocityLifeFade;
	ParticleFadeProperty GravityLifeFade;
	
	int MaxParticles;
	float ParticleRate;

	bool HasKillBox;
	bool KeepParticlesLocal;
};

class ParticleGenerationSystem : public ISystem
{
public:
	virtual void InitializeSystem(Scene& scene) override;
	virtual void Update(float deltaTime) override;
	virtual std::string GetName() override;
	virtual std::vector<std::string> GetSystemsBefore() override;
	virtual std::vector<std::string> GetSystemsAfter() override;
	virtual bool IsMainThread() override;
private:
	Scene *m_scene;
};

