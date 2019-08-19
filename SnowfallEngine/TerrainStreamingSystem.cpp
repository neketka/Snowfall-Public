#include "stdafx.h"
#include "TerrainStreamingSystem.h"
#include "PhysicsWorldSystem.h"
#include "Noise.h"

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

			glm::vec2 camChunkPos;

			TransformComponent *cam = comp->StreamingCamera.GetComponent<TransformComponent>();
			if (cam)
			{
				camChunkPos = glm::round((glm::vec2(cam->GlobalPosition.x, cam->GlobalPosition.z) + glm::vec2(0.5f)) * (1 / (float(comp->Terrain->GetRealLength()) - 1))) - glm::vec2(0.5f);
			}

			std::vector<int> addresses;

			for (int y = 0; y <= comp->MaxStreamDistance * 2; ++y)
			{
				for (int x = 0; x <= comp->MaxStreamDistance * 2; ++x)
				{
					glm::vec2 chunk = glm::vec2(x, y) + camChunkPos + glm::vec2(-comp->MaxStreamDistance + 0.5f);
					float dist = glm::length(camChunkPos - chunk) * (LOD_COUNT - 2) / comp->MaxStreamDistance;

					comp->Terrain->RequestChunkMaxLOD(chunk.x, chunk.y, glm::min<int>(3, dist));
					addresses.push_back(comp->Terrain->ChunkToAddress(chunk));
				}
			}
			comp->Terrain->UnloadChunks(addresses, true);

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
			comp->Terrain->RenderTerrainToMeshManager(comp->LayerMask);

			int size = comp->Terrain->GetBaseLength() - 1;
			for (TerrainLoadUpdate& upd : comp->Terrain->GetLoadUpdates())
			{
				if (!upd.NewlyCreated)
					continue;
				comp->Terrain->ApplyOperation([](float& height, glm::vec4& alpha, float& lowerBound, glm::vec2 chunkVertexPos, glm::ivec2 chunkVertexIndex, glm::ivec2 chunkPos) {
					lowerBound = FractalSimplex(3, chunkVertexPos, 4.f, 2.f, 0.003f, 0, 64);
					height = lowerBound + 5;
					alpha = glm::vec4(1, 0, 0, 0);
				}, upd.ChunkAddress, true);
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