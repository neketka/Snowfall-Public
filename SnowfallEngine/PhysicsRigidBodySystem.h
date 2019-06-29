#pragma once
#include "ECS.h"
#include "PhysicsWorldSystem.h"
#include "MeshAsset.h"
#include <glm/glm.hpp>

enum class CollisionShapeType
{
	Unassigned, Box, Sphere, Mesh, Cylindrical, Cone, Heightfield
};

class PhysicsRigidBodyComponent : public Component
{
public:
	float LinearDamping = 0.01f;
	float AngularDamping = 0.01f;

	glm::vec3 LinearFactor = glm::vec3(1);
	glm::vec3 AngularFactor = glm::vec3(1);

	glm::vec3 LinearVelocity;
	glm::vec3 AngularVelocity;

	float Mass;
	glm::vec3 Inertia;

	float RollingFriction = 0.1f;
	float Friction = 0.5f;
	float Restitution = 0.5f;

	CollisionShapeType ShapeType;
	glm::mat4 ShapeTransform;

	btRigidBody *rigidBody;
	CollisionShapeType prevType;
};

class StridingMeshInterface : public btStridingMeshInterface
{
public:
	StridingMeshInterface(MeshAsset *asset);

	virtual void getLockedVertexIndexBase(unsigned char **vertexbase, int& numverts, PHY_ScalarType& type, int& stride, unsigned char **indexbase, int& indexstride, int& numfaces, PHY_ScalarType& indicestype, int subPart) override;
	virtual void getLockedReadOnlyVertexIndexBase(const unsigned char **vertexbase, int& numverts, PHY_ScalarType& type, int& stride, const unsigned char **indexbase, int& indexstride, int& numfaces, PHY_ScalarType& indicestype, int subPart) const override;

	virtual void unLockVertexBase(int subpart) override;
	virtual void unLockReadOnlyVertexBase(int subpart) const override;

	virtual int getNumSubParts() const override { return 1; }

	virtual void preallocateVertices(int numverts) override;
	virtual void preallocateIndices(int numindices) override;
private:
	MeshAsset *m_asset;
};

class PhysicsBoxCollisionComponent : public Component
{
public:
	glm::vec3 Size;

	btCollisionShape *shape;
};

class PhysicsSphereCollisionComponent : public Component
{
public:
	float Radius;

	btCollisionShape *shape;
};

class PhysicsMeshCollisionComponent : public Component
{
public:
	MeshAsset *Mesh;
	bool IsConvexHull;

	btCollisionShape *shape;
};

class PhysicsCylinderCollisionComponent : public Component
{
public:
	float Radius;
	float Height;

	bool IsCapsule;

	btCollisionShape *shape;
};

class PhysicsConeCollisionComponent : public Component
{
public:
	float Height;
	float Radius;

	btCollisionShape *shape;
};

class PhysicsHeightfieldCollisionComponent : public Component
{
public:
};

class SNOWFALLENGINE_API PhysicsRigidBodySystem : public ISystem
{
public:
	virtual ~PhysicsRigidBodySystem() override;
	virtual void InitializeSystem(Scene& scene) override;
	virtual void Update(float deltaTime) override;
	virtual std::string GetName() override;
	virtual std::vector<std::string> GetSystemsBefore() override;
	virtual std::vector<std::string> GetSystemsAfter() override;
	virtual bool IsMainThread() override;

	void UpdateRigidbodiesTransform();
private:
	bool BuildRigidbody(PhysicsRigidBodyComponent *comp);

	Scene *m_scene;
};

