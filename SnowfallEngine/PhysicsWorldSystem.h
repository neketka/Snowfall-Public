#pragma once
#include "ECS.h"
#include <btBulletDynamicsCommon.h>

class PhysicsHandle
{
public:
	btDefaultCollisionConfiguration *CollisionConfiguration;
	btCollisionDispatcher *Dispatcher;
	btBroadphaseInterface *OverlappingPairCache;
	btSequentialImpulseConstraintSolver *Solver;
	btDiscreteDynamicsWorld *DynamicsWorld;
};

class SNOWFALLENGINE_API PhysicsWorldSystem : public ISystem
{
public:
	PhysicsWorldSystem();
	virtual ~PhysicsWorldSystem() override;
	virtual void InitializeSystem(Scene& scene) override;
	virtual void Update(float deltaTime) override;
	virtual std::string GetName() override;
	virtual std::vector<std::string> GetSystemsBefore() override;
	virtual std::vector<std::string> GetSystemsAfter() override;
	virtual bool IsMainThread() override;

	PhysicsHandle *GetPhysicsHandle();
	void SetGravity(glm::vec3 gravity);
private:
	PhysicsHandle m_handle;

	Scene *m_scene;
};
