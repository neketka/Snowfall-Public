#include "stdafx.h"
#include "PhysicsWorldSystem.h"
#include "PhysicsRigidBodySystem.h"
#include "TerrainStreamingSystem.h"

PhysicsWorldSystem::PhysicsWorldSystem()
{
}

PhysicsWorldSystem::~PhysicsWorldSystem()
{
	btCollisionObjectArray& arr = m_handle.DynamicsWorld->getCollisionObjectArray();
	for (int i = 0; i < arr.size(); ++i)
	{
		btCollisionObject *obj = arr[i];
		btRigidBody *rigidBody = btRigidBody::upcast(obj);

		if (!rigidBody)
			continue;

		m_handle.DynamicsWorld->removeRigidBody(rigidBody);

		auto inter = dynamic_cast<btBvhTriangleMeshShape *>(rigidBody->getCollisionShape());
		if (inter)
			delete inter->getMeshInterface();

		delete rigidBody->getCollisionShape();
		delete rigidBody->getMotionState();

		delete rigidBody;
	}

	delete m_handle.DynamicsWorld;
	delete m_handle.Solver;
	delete m_handle.OverlappingPairCache;
	delete m_handle.Dispatcher;
	delete m_handle.CollisionConfiguration;
}

void myTickCallback(btDynamicsWorld *world, btScalar timeStep) {
	int numManifolds = world->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = world->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* obA = const_cast<btCollisionObject*>(contactManifold->getBody0());
		btCollisionObject* obB = const_cast<btCollisionObject*>(contactManifold->getBody1());

		int numContacts = contactManifold->getNumContacts();
		for (int j = 0; j < numContacts; j++)
		{
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			if (pt.getDistance() < 0.f)
			{
				const btVector3& ptA = pt.getPositionWorldOnA();
				const btVector3& ptB = pt.getPositionWorldOnB();
				const btVector3& normalOnB = pt.m_normalWorldOnB;
				std::string comp1 = reinterpret_cast<Component *>(obA->getUserPointer())->InternalName;
				std::string comp2 = reinterpret_cast<Component *>(obB->getUserPointer())->InternalName;
				Snowfall::GetGameInstance().Log(LogType::Message, "Collision: " + comp1 + " " + comp2);
			}
		}
	}
}

void PhysicsWorldSystem::InitializeSystem(Scene& scene)
{
	m_scene = &scene;

	m_handle.CollisionConfiguration = new btDefaultCollisionConfiguration;
	m_handle.Dispatcher = new btCollisionDispatcher(m_handle.CollisionConfiguration);
	m_handle.OverlappingPairCache = new btDbvtBroadphase;
	m_handle.Solver = new btSequentialImpulseConstraintSolver;
	
	m_handle.DynamicsWorld = new btDiscreteDynamicsWorld(m_handle.Dispatcher, m_handle.OverlappingPairCache, 
		m_handle.Solver, m_handle.CollisionConfiguration);
	//m_handle.DynamicsWorld->setInternalTickCallback(myTickCallback);

	SetGravity(glm::vec3(0, -9.81f, 0));
}

void PhysicsWorldSystem::Update(float deltaTime)
{
	m_handle.DynamicsWorld->stepSimulation(deltaTime);
	PhysicsRigidBodySystem *rigid = dynamic_cast<PhysicsRigidBodySystem *>(m_scene->GetSystemManager().GetSystem("PhysicsRigidBodySystem"));
	if (rigid)
		rigid->UpdateRigidbodiesTransform();
}

std::string PhysicsWorldSystem::GetName()
{
	return "PhysicsWorldSystem";
}

std::vector<std::string> PhysicsWorldSystem::GetSystemsBefore()
{
	return std::vector<std::string>();
}

std::vector<std::string> PhysicsWorldSystem::GetSystemsAfter()
{
	return { "TransformSystem" };
}

bool PhysicsWorldSystem::IsMainThread()
{
	return false;
}

PhysicsHandle *PhysicsWorldSystem::GetPhysicsHandle()
{
	return &m_handle;
}

void PhysicsWorldSystem::SetGravity(glm::vec3 gravity)
{
	m_handle.DynamicsWorld->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
}
