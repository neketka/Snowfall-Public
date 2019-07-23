#include "stdafx.h"
#include "TerrainStreamingSystem.h"
#include "PhysicsWorldSystem.h"
#include <glm/gtc/noise.hpp>

TerrainStreamingSystem::TerrainStreamingSystem()
{
}

TerrainStreamingSystem::~TerrainStreamingSystem()
{
}

void TerrainStreamingSystem::InitializeSystem(Scene& scene)
{
	m_scene = &scene;
}

void TerrainStreamingSystem::Update(float deltaTime)
{
	for (TerrainComponent *comp : m_scene->GetComponentManager().GetComponents<TerrainComponent>())
	{
		if (comp->Terrain)
		{
			comp->Terrain->ClearLodUpdates();

			comp->Terrain->RequestChunkMaxLOD(0, 0, 0);
			comp->Terrain->RequestChunkMaxLOD(1, 0, 0);
			comp->Terrain->RequestChunkMaxLOD(0, 1, 0);

			comp->Terrain->Load();

			for (TerrainLoadUpdate& upd : comp->Terrain->GetLoadUpdates())
			{
				PhysicsWorldSystem *physWorld = m_scene->GetSystemManager().GetSystem<PhysicsWorldSystem>();
				if (physWorld)
				{
					PhysicsHandle *handle = physWorld->GetPhysicsHandle();
					if (comp->physicsObjects.contains(upd.ChunkAddress))
					{
						handle->DynamicsWorld->removeCollisionObject(comp->physicsObjects[upd.ChunkAddress]);
						delete comp->physicsObjects[upd.ChunkAddress];
					}
					if (upd.NewLod == -1)
						comp->physicsObjects.erase(upd.ChunkAddress);
					else
					{
						btCollisionObject *obj = comp->physicsObjects[upd.ChunkAddress] = new btCollisionObject;
						auto shape = comp->Terrain->GetPhysicsShape(upd.ChunkAddress);

						obj->setUserPointer(comp);
						obj->setCollisionShape(shape);
						//obj->setCcdMotionThreshold(1e-7f);
						//obj->setCcdSweptSphereRadius(0.5f);

						btTransform& trans = obj->getWorldTransform();

						float sc = 0;
						trans.setOrigin(btVector3(comp->Terrain->GetRealLength() / (2) - sc + upd.ChunkX * (comp->Terrain->GetRealLength() - 1),
							(comp->Terrain->GetMaxHeight() - comp->Terrain->GetMinHeight()) / 2 - comp->Terrain->GetMinHeight(), 
							comp->Terrain->GetRealLength() / (2) - sc + upd.ChunkY * (comp->Terrain->GetRealLength() - 1)));

						handle->DynamicsWorld->addCollisionObject(obj);
					}
				}
			}

			comp->Terrain->ApplyChangesToGPU();
			comp->Terrain->RenderTerrainToMeshManager(0xFFFFFFFFFFFFFFFF);

			int size = comp->Terrain->GetBaseLength() - 1;
			for (TerrainLoadUpdate& upd : comp->Terrain->GetLoadUpdates())
			{
				if (!upd.NewlyCreated)
					continue;
				std::vector<glm::vec4> alpha(257 * 257);
				std::vector<float> heights(257 * 257);
				for (int y = 0; y < 257; ++y)
				{
					for (int x = 0; x < 257; ++x)
					{
						float kX = (x + size * upd.ChunkX);
						float kY = (y + size * upd.ChunkY);

						bool d = glm::simplex(glm::vec3(kX, kY, Snowfall::GetGameInstance().GetTime())) > 0;
						alpha[y * 257 + x] = d ? glm::vec4(1, 0, 0, 0) : glm::vec4(0, 1, 0, 0);
						heights[y * 257 + x] = (glm::simplex(glm::vec2(kX * 0.01f, kY * 0.01f)) + 1) * 16.f;
					}
				}

				comp->Terrain->ModifyArea(upd.ChunkAddress, heights.data());
				comp->Terrain->ModifyAlphaMap(upd.ChunkAddress, alpha.data());
			}
		}
	}
}

std::string TerrainStreamingSystem::GetName()
{
	return "TerrainStreamingSystem";
}

std::vector<std::string> TerrainStreamingSystem::GetSystemsBefore()
{
	return { "TransformSystem" };
}

std::vector<std::string> TerrainStreamingSystem::GetSystemsAfter()
{
	return { "CameraSystem" };
}

bool TerrainStreamingSystem::IsMainThread()
{
	return true;
}

std::vector<SerializationField> ComponentDescriptor<TerrainComponent>::GetSerializationFields()
{
	return {
	};
}