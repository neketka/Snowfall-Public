#pragma once
#include "ECS.h"
#include "TerrainAsset.h"
#include <BulletCollision/CollisionDispatch/btCollisionObject.h>

class TerrainComponent : public Component
{
public:
	TerrainAsset *Terrain;

	std::map<int, btCollisionObject *> physicsObjects;
};

class TerrainStreamingSystem : public ISystem
{
public:
	TerrainStreamingSystem();
	virtual ~TerrainStreamingSystem() override;
	virtual void InitializeSystem(Scene & scene) override;
	virtual void Update(float deltaTime) override;
	virtual std::string GetName() override;
	virtual std::vector<std::string> GetSystemsBefore() override;
	virtual std::vector<std::string> GetSystemsAfter() override;
	virtual bool IsMainThread() override;
private:
	Scene *m_scene;
};

