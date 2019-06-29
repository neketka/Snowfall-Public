#include "stdafx.h"
#include "PhysicsRigidBodySystem.h"
#include "PhysicsWorldSystem.h"
#include "MeshComponent.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Mesh.h"

PhysicsRigidBodySystem::~PhysicsRigidBodySystem()
{
}

void PhysicsRigidBodySystem::InitializeSystem(Scene& scene)
{
	m_scene = &scene;
}

void PhysicsRigidBodySystem::Update(float deltaTime)
{
	for (PhysicsRigidBodyComponent *comp : m_scene->GetComponentManager().GetComponents<PhysicsRigidBodyComponent>())
	{
		if (!comp->rigidBody)
			if (!BuildRigidbody(comp))
				continue;

		TransformComponent *transformComp = comp->Owner.GetComponent<TransformComponent>();

		auto scale = btVector3(
			glm::length(glm::vec3(transformComp->ModelMatrix[0])),
			glm::length(glm::vec3(transformComp->ModelMatrix[1])),
			glm::length(glm::vec3(transformComp->ModelMatrix[2])));

		comp->rigidBody->getCollisionShape()->setLocalScaling(scale);
		comp->rigidBody->setDamping(comp->LinearDamping, comp->AngularDamping);
		comp->rigidBody->setLinearVelocity(btVector3(comp->LinearVelocity.x, comp->LinearVelocity.y, comp->LinearVelocity.z));
		comp->rigidBody->setAngularVelocity(btVector3(comp->AngularVelocity.x, comp->AngularVelocity.y, comp->AngularVelocity.z));
		comp->rigidBody->setRollingFriction(comp->RollingFriction);
		comp->rigidBody->setFriction(comp->Friction);
		comp->rigidBody->setRestitution(comp->Restitution);
		comp->rigidBody->setLinearFactor(btVector3(comp->LinearFactor.x, comp->LinearFactor.y, comp->LinearFactor.z));
		comp->rigidBody->setAngularFactor(btVector3(comp->AngularFactor.x, comp->AngularFactor.y, comp->AngularFactor.z));

		glm::mat4 transf = glm::scale(comp->ShapeTransform * transformComp->ModelMatrix, glm::vec3(1.f / scale.x(), 1.f / scale.y(), 1.f / scale.z()));
		
		btTransform trans;
		trans.setFromOpenGLMatrix(reinterpret_cast<btScalar *>(&transf));
	
		comp->rigidBody->setWorldTransform(trans);
	}

	for (PhysicsRigidBodyComponent *comp : m_scene->GetComponentManager().GetDeadComponents<PhysicsRigidBodyComponent>())
	{
		if (comp->rigidBody)
		{
			dynamic_cast<PhysicsWorldSystem *>(m_scene->GetSystemManager().GetSystem("PhysicsWorldSystem"))
				->GetPhysicsHandle()->DynamicsWorld->removeRigidBody(comp->rigidBody);
			delete comp->rigidBody->getCollisionShape();
			delete comp->rigidBody->getMotionState();
			if (comp->ShapeType == CollisionShapeType::Mesh)
				delete dynamic_cast<btBvhTriangleMeshShape *>(comp->rigidBody->getCollisionShape())->getMeshInterface();
			delete comp->rigidBody;
		}
	}
}

std::string PhysicsRigidBodySystem::GetName()
{
	return "PhysicsRigidBodySystem";
}

std::vector<std::string> PhysicsRigidBodySystem::GetSystemsBefore()
{
	return { "TransformSystem" };
}

std::vector<std::string> PhysicsRigidBodySystem::GetSystemsAfter()
{
	return { "PhysicsWorldSystem" };
}

bool PhysicsRigidBodySystem::IsMainThread()
{
	return false;
}

bool PhysicsRigidBodySystem::BuildRigidbody(PhysicsRigidBodyComponent *comp)
{
	btCollisionShape *shape = nullptr;

	PhysicsBoxCollisionComponent *box;
	PhysicsConeCollisionComponent *cone;
	PhysicsCylinderCollisionComponent *cylinder;
	PhysicsMeshCollisionComponent *mesh;
	PhysicsSphereCollisionComponent *sphere;
	MeshRenderComponent *rMesh;

	MeshAsset *ms = nullptr;

	glm::vec3 temp;
	glm::vec3 temp1;

	switch (comp->ShapeType)
	{
	case CollisionShapeType::Box:
		box = comp->Owner.GetComponent<PhysicsBoxCollisionComponent>();
		if (!box)
			return false;
		temp = box->Size / 2.f;
		box->shape = shape = new btBoxShape(btVector3(temp.x, temp.y, temp.z));
		break;
	case CollisionShapeType::Cone:
		cone = comp->Owner.GetComponent<PhysicsConeCollisionComponent>();
		if (!cone)
			return false;
		shape = new btConeShape(cone->Radius, cone->Height);
		break;
	case CollisionShapeType::Cylindrical:
		cylinder = comp->Owner.GetComponent<PhysicsCylinderCollisionComponent>();
		if (!cylinder)
			return false;
		if (cylinder->IsCapsule)
			cylinder->shape = shape = new btCapsuleShape(cylinder->Radius, cylinder->Height);
		else
			cylinder->shape = shape = new btCylinderShape(btVector3(cylinder->Height, cylinder->Radius, cylinder->Radius));
		break;
	case CollisionShapeType::Heightfield:
		break;
	case CollisionShapeType::Mesh:
		mesh = comp->Owner.GetComponent<PhysicsMeshCollisionComponent>();
		if (!mesh)
			return false;
		if (!mesh->Mesh)
		{
			rMesh = comp->Owner.GetComponent<MeshRenderComponent>();
			if (!rMesh || !rMesh->Mesh)
				return false;
			ms = rMesh->Mesh;
		}

		temp = ms->GetBoundingBox().MinExtent;
		temp1 = ms->GetBoundingBox().MaxExtent;

		if (mesh->IsConvexHull)
		{
			ms->Load();
			mesh->shape = shape = new btConvexHullShape(reinterpret_cast<btScalar *>(ms->GetMesh().Vertices.data()), ms->GetMesh().Vertices.size(), sizeof(RenderVertex));
		}
		else
		{
			mesh->shape = shape = new btBvhTriangleMeshShape(new StridingMeshInterface(ms), true, btVector3(temp.x, temp.y, temp.z), btVector3(temp1.x, temp1.y, temp1.z));
		}
		break;
	case CollisionShapeType::Sphere:
		sphere = comp->Owner.GetComponent<PhysicsSphereCollisionComponent>();
		if (!sphere)
			return false;

		sphere->shape = shape = new btSphereShape(sphere->Radius);
		break;
	default:
		return false;
	}

	btVector3 inertia(0, 0, 0);
	if (comp->Mass != 0)
		shape->calculateLocalInertia(comp->Mass, inertia);

	comp->prevType = comp->ShapeType;
	comp->Inertia = glm::vec3(inertia.x(), inertia.y(), inertia.z());

	btDefaultMotionState *myMotionState = new btDefaultMotionState();
	btRigidBody::btRigidBodyConstructionInfo rbInfo(comp->Mass, myMotionState, shape, inertia);
	btRigidBody *body = comp->rigidBody = new btRigidBody(rbInfo);
	body->setUserPointer(comp);

	dynamic_cast<PhysicsWorldSystem *>(m_scene->GetSystemManager().GetSystem("PhysicsWorldSystem"))->GetPhysicsHandle()->DynamicsWorld->addRigidBody(body);

	return true;
}

void PhysicsRigidBodySystem::UpdateRigidbodiesTransform()
{
	for (PhysicsRigidBodyComponent *comp : m_scene->GetComponentManager().GetComponents<PhysicsRigidBodyComponent>())
	{
		if (comp->rigidBody)
		{
			TransformComponent *transformComp = comp->Owner.GetComponent<TransformComponent>();

			btVector3 lvel = comp->rigidBody->getLinearVelocity();
			btVector3 avel = comp->rigidBody->getAngularVelocity();

			glm::mat4 inv = glm::inverse(comp->ShapeTransform);

			btTransform trans = comp->rigidBody->getWorldTransform();
			btTransform invTrans;
			invTrans.setFromOpenGLMatrix(reinterpret_cast<btScalar *>(&inv));

			trans.mult(trans, invTrans);

			btVector3 org = trans.getOrigin();

			auto scale = glm::vec3(
				glm::length(glm::vec3(transformComp->ModelMatrix[0])),
				glm::length(glm::vec3(transformComp->ModelMatrix[1])),
				glm::length(glm::vec3(transformComp->ModelMatrix[2])));

			trans.getOpenGLMatrix(reinterpret_cast<btScalar *>(&transformComp->ModelMatrix));
			transformComp->ModelMatrix = glm::scale(transformComp->ModelMatrix, scale);
			trans.getRotation().getEulerZYX(transformComp->GlobalRotation.z, transformComp->GlobalRotation.y, transformComp->GlobalRotation.x);
			transformComp->GlobalPosition = glm::vec3(org.x(), org.y(), org.z());

			transformComp->GlobalDirection = -glm::vec3(transformComp->ModelMatrix[0][2], transformComp->ModelMatrix[1][2], transformComp->ModelMatrix[2][2]);
			comp->LinearVelocity = glm::vec3(lvel.x(), lvel.y(), lvel.z());
			comp->AngularVelocity = glm::vec3(avel.x(), avel.y(), avel.z());

			glm::mat4 t = glm::inverse(transformComp->parentMatrix);

			btTransform local;
			local.setFromOpenGLMatrix(reinterpret_cast<btScalar *>(&t));
			local.mult(trans, local);
			org = local.getOrigin();
			local.getRotation().getEulerZYX(transformComp->Rotation.z, transformComp->Rotation.y, transformComp->Rotation.x);
			transformComp->GlobalRotation *= 180.f / 3.14159265f;
			transformComp->Rotation *= 180.f / 3.14159265f;
			transformComp->Position = glm::vec3(org.x(), org.y(), org.z());
		}
	}
}

StridingMeshInterface::StridingMeshInterface(MeshAsset *asset)
{
	m_asset = asset;
}

void StridingMeshInterface::getLockedVertexIndexBase(unsigned char **vertexbase, int& numverts, PHY_ScalarType& type, int& stride, unsigned char **indexbase, int& indexstride, int& numfaces, PHY_ScalarType& indicestype, int subPart)
{
	getLockedReadOnlyVertexIndexBase(const_cast<const unsigned char **>(vertexbase), numverts, type, stride, const_cast<const unsigned char **>(indexbase), indexstride, numfaces, indicestype, subPart);
}

void StridingMeshInterface::getLockedReadOnlyVertexIndexBase(const unsigned char **vertexbase, int& numverts, PHY_ScalarType& type, int& stride, const unsigned char **indexbase, int& indexstride, int& numfaces, PHY_ScalarType& indicestype, int subPart) const
{
	m_asset->Load();
	Mesh& mesh = m_asset->GetMesh();

	*vertexbase = reinterpret_cast<unsigned char *>(mesh.Vertices.data());
	numverts = mesh.Vertices.size();
	type = PHY_ScalarType::PHY_FLOAT;
	stride = sizeof(RenderVertex);
	*indexbase = reinterpret_cast<unsigned char *>(mesh.Indices.data());
	indexstride = sizeof(int) * 3;
	numfaces = mesh.Indices.size() / 3;
	indicestype = PHY_ScalarType::PHY_INTEGER;
}

void StridingMeshInterface::unLockVertexBase(int subpart)
{
}

void StridingMeshInterface::unLockReadOnlyVertexBase(int subpart) const
{
}

void StridingMeshInterface::preallocateVertices(int numverts)
{
}

void StridingMeshInterface::preallocateIndices(int numindices)
{
}

std::vector<SerializationField> ComponentDescriptor<PhysicsRigidBodyComponent>::GetSerializationFields()
{
	return {
		SerializationField("LinearDamping", SerializationType::ByValue, offsetof(PhysicsRigidBodyComponent, LinearDamping), sizeof(float), InterpretValueAs::Float32),
		SerializationField("AngularDamping", SerializationType::ByValue, offsetof(PhysicsRigidBodyComponent, AngularDamping), sizeof(float), InterpretValueAs::Float32),
		SerializationField("Mass", SerializationType::ByValue, offsetof(PhysicsRigidBodyComponent, Mass), sizeof(float), InterpretValueAs::Float32),
		SerializationField("Inertia", SerializationType::ByValue, offsetof(PhysicsRigidBodyComponent, Inertia), sizeof(glm::vec3), InterpretValueAs::FVector3),
		SerializationField("RollingFriction", SerializationType::ByValue, offsetof(PhysicsRigidBodyComponent, RollingFriction), sizeof(float), InterpretValueAs::Float32),
		SerializationField("Friction", SerializationType::ByValue, offsetof(PhysicsRigidBodyComponent, Friction), sizeof(float), InterpretValueAs::Float32),
		SerializationField("Restitution", SerializationType::ByValue, offsetof(PhysicsRigidBodyComponent, Restitution), sizeof(float), InterpretValueAs::Float32),
		SerializationField("LinearVelocity", SerializationType::ByValue, offsetof(PhysicsRigidBodyComponent, LinearVelocity), sizeof(glm::vec3), InterpretValueAs::FVector3),
		SerializationField("AngularVelocity", SerializationType::ByValue, offsetof(PhysicsRigidBodyComponent, AngularVelocity), sizeof(glm::vec3), InterpretValueAs::FVector3),
		SerializationField("ShapeType", SerializationType::ByValue, offsetof(PhysicsRigidBodyComponent, ShapeType), sizeof(CollisionShapeType), InterpretValueAs::UInt32),
		SerializationField("ShapeTransform", SerializationType::ByValue, offsetof(PhysicsRigidBodyComponent, ShapeTransform), sizeof(glm::mat4), InterpretValueAs::FMatrix4),
	};
}

std::vector<SerializationField> ComponentDescriptor<PhysicsBoxCollisionComponent>::GetSerializationFields()
{
	return {
		SerializationField("Size", SerializationType::ByValue, offsetof(PhysicsBoxCollisionComponent, Size), sizeof(glm::vec3), InterpretValueAs::FVector3),
	};
}

std::vector<SerializationField> ComponentDescriptor<PhysicsSphereCollisionComponent>::GetSerializationFields()
{
	return {
		SerializationField("Radius", SerializationType::ByValue, offsetof(PhysicsSphereCollisionComponent, Radius), sizeof(float), InterpretValueAs::Float32),
	};
}

std::vector<SerializationField> ComponentDescriptor<PhysicsMeshCollisionComponent>::GetSerializationFields()
{
	return {
		SerializationField("Mesh", SerializationType::Asset, offsetof(PhysicsMeshCollisionComponent, Mesh), sizeof(glm::vec3), InterpretValueAs::FVector3),
		SerializationField("IsConvexHull", SerializationType::ByValue, offsetof(PhysicsMeshCollisionComponent, IsConvexHull), sizeof(bool), InterpretValueAs::Bool),
	};
}

std::vector<SerializationField> ComponentDescriptor<PhysicsCylinderCollisionComponent>::GetSerializationFields()
{
	return {
		SerializationField("Radius", SerializationType::ByValue, offsetof(PhysicsCylinderCollisionComponent, Radius), sizeof(float), InterpretValueAs::Float32),
		SerializationField("Height", SerializationType::ByValue, offsetof(PhysicsCylinderCollisionComponent, Height), sizeof(float), InterpretValueAs::Float32),
		SerializationField("IsCapsule", SerializationType::ByValue, offsetof(PhysicsCylinderCollisionComponent, IsCapsule), sizeof(bool), InterpretValueAs::Bool),
	};
}

std::vector<SerializationField> ComponentDescriptor<PhysicsConeCollisionComponent>::GetSerializationFields()
{
	return {
		SerializationField("Height", SerializationType::ByValue, offsetof(PhysicsConeCollisionComponent, Height), sizeof(float), InterpretValueAs::Float32),
		SerializationField("Radius", SerializationType::ByValue, offsetof(PhysicsConeCollisionComponent, Radius), sizeof(float), InterpretValueAs::Float32),
	};
}

std::vector<SerializationField> ComponentDescriptor<PhysicsHeightfieldCollisionComponent>::GetSerializationFields()
{
	return {

	};
}