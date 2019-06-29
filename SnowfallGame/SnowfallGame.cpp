#include "module.h"

#include <Snowfall.h>
#include <TextureAsset.h>
#include <MeshAsset.h>

#include <TransformComponent.h>
#include <MeshComponent.h>
#include <CameraComponent.h>
#include <LightComponent.h>
#include <SkyboxComponent.h>
#include <CameraUIRenderSystem.h>
#include <LocalAssetStream.h>
#include <PhysicsRigidBodySystem.h>

#include "CameraFlyComponent.h"
#include "TestComponent.h"
#include "TestUI.h"

#include <glm/gtc/matrix_transform.hpp>

Scene *scene;

void SnowfallGame::InitializeModule()
{
	Snowfall::GetGameInstance().CreateViewport(0);
	Snowfall::GetGameInstance().SetViewportCoefficients(0, Quad2D(0, 0, 1.f, 1.f), Quad2D(0, 0, 0, 0));

	Sampler sampler;

	Material material;
	material.MaterialShader = &AssetManager::LocateAssetGlobal<ShaderAsset>("SimplePBRShader");
	material.PerObjectParameterCount = 1;

	Snowfall::GetGameInstance().GetAssetManager().AddAsset(new MaterialAsset("FortMaterial", { sampler },
		{ &AssetManager::LocateAssetGlobal<TextureAsset>("fortnite") }, { SamplerProperty(0, 0, 16) }, material));

	Snowfall::GetGameInstance().GetAssetManager().AddAsset(new MaterialAsset("TonemappingMaterial",
		Material(&AssetManager::LocateAssetGlobal<ShaderAsset>("TonemappingPost"))));

	Snowfall::GetGameInstance().GetAssetManager().AddAsset(new MaterialAsset("FxaaMaterial",
		Material(&AssetManager::LocateAssetGlobal<ShaderAsset>("FxaaPost"))));

	Snowfall::GetGameInstance().GetAssetManager().AddAsset(new MaterialAsset("BloomMaterial",
		Material(&AssetManager::LocateAssetGlobal<ShaderAsset>("BloomPost"))));

	scene = new Scene;

	scene->GetSystemManager().AddEnabledSystems({
		"TransformSystem", "PhysicsRigidBodySystem", "PhysicsWorldSystem", "TestSystem", "MeshRenderingSystem", "ShadowMapRenderSystem",
		"LightSystem", "CameraSystem", "PostProcessRenderSystem", "CameraUIRenderSystem",
		"CameraViewportRenderSystem", "CameraFlySystem" 
	});

	Entity camera = scene->GetEntityManager().CreateEntity({ "TransformComponent", "CameraComponent", "CameraFlyComponent", "CameraUIRenderComponent" });
	camera.GetComponent<TransformComponent>()->Position = glm::vec3(0, 5, 5);
	camera.GetComponent<CameraComponent>()->ViewportIndex = 0;
	camera.GetComponent<CameraComponent>()->FovY = 90;
	camera.GetComponent<CameraUIRenderComponent>()->ContextName = "TestUI";

	camera.GetComponent<CameraComponent>()->PostProcessStack.push_back(&AssetManager::LocateAssetGlobal<MaterialAsset>("FxaaMaterial"));
	camera.GetComponent<CameraComponent>()->PostProcessStack.push_back(&AssetManager::LocateAssetGlobal<MaterialAsset>("BloomMaterial"));
	camera.GetComponent<CameraComponent>()->PostProcessStack.push_back(&AssetManager::LocateAssetGlobal<MaterialAsset>("TonemappingMaterial"));

	Entity skybox = scene->GetEntityManager().CreateEntity({ "SkyboxComponent" });
	skybox.GetComponent<SkyboxComponent>()->Cubemap = &AssetManager::LocateAssetGlobal<TextureAsset>("Daylight");

	Entity e = scene->GetEntityManager().CreateEntity({ "TransformComponent", "MeshRenderComponent", "PhysicsRigidBodyComponent", "PhysicsBoxCollisionComponent" });
	
	e.GetComponent<TransformComponent>()->Position = glm::vec3(1.5f, 3.f, 1.5f);
	e.GetComponent<TransformComponent>()->Scale = glm::vec3(0.5f, 0.5f, 0.5f);
	e.GetComponent<MeshRenderComponent>()->Material = &AssetManager::LocateAssetGlobal<MaterialAsset>("FortMaterial");
	e.GetComponent<MeshRenderComponent>()->Mesh = &AssetManager::LocateAssetGlobal<MeshAsset>("Cube.Cube_Cube.002");
	e.GetComponent<MeshRenderComponent>()->ObjectParameters = { glm::vec4(0.0f, 0.5f, 1, 1) };
	e.GetComponent<MeshRenderComponent>()->BatchingType = BatchingType::Instanced;
	e.GetComponent<PhysicsRigidBodyComponent>()->ShapeTransform = glm::mat4();
	e.GetComponent<PhysicsRigidBodyComponent>()->ShapeType = CollisionShapeType::Box;
	e.GetComponent<PhysicsRigidBodyComponent>()->Mass = 1.f;
	e.GetComponent<PhysicsRigidBodyComponent>()->AngularVelocity = glm::vec3(1, 1, 0);
	e.GetComponent<PhysicsBoxCollisionComponent>()->Size = glm::vec3(2, 2, 2);

	Entity ground = scene->GetEntityManager().CreateEntity({ "TransformComponent", "MeshRenderComponent", "PhysicsRigidBodyComponent", "PhysicsBoxCollisionComponent" });

	ground.GetComponent<TransformComponent>()->Position = glm::vec3(1.5f, 0.f, 1.5f);
	ground.GetComponent<TransformComponent>()->Scale = glm::vec3(5.f, 0.5f, 5.f);
	ground.GetComponent<MeshRenderComponent>()->Material = &AssetManager::LocateAssetGlobal<MaterialAsset>("FortMaterial");
	ground.GetComponent<MeshRenderComponent>()->Mesh = &AssetManager::LocateAssetGlobal<MeshAsset>("Cube.Cube_Cube.002");
	ground.GetComponent<MeshRenderComponent>()->ObjectParameters = { glm::vec4(0.0f, 0.5f, 1, 1) };
	ground.GetComponent<MeshRenderComponent>()->BatchingType = BatchingType::Instanced;
	ground.GetComponent<PhysicsRigidBodyComponent>()->ShapeTransform = glm::mat4();
	ground.GetComponent<PhysicsRigidBodyComponent>()->ShapeType = CollisionShapeType::Box;
	ground.GetComponent<PhysicsRigidBodyComponent>()->Mass = 0.f;
	ground.GetComponent<PhysicsBoxCollisionComponent>()->Size = glm::vec3(2, 2, 2);

	Entity light0 = scene->GetEntityManager().CreateEntity({ "TransformComponent", "LightComponent" });
				  
	light0.GetComponent<TransformComponent>()->Position = glm::vec3(0, 5, 5);
	light0.GetComponent<TransformComponent>()->Rotation = glm::vec3(45, 0, 0);
	light0.GetComponent<LightComponent>()->Enabled = true;
	light0.GetComponent<LightComponent>()->Shadowing = true;
	light0.GetComponent<LightComponent>()->Intensity = 3.f;
	light0.GetComponent<LightComponent>()->ShadowLayerMask = 0xFFFFFFFFFFFFFFFF;
	light0.GetComponent<LightComponent>()->Type = LightType::Directional;
	light0.GetComponent<LightComponent>()->Color = glm::vec3(1, 1, 1);

	IAssetStreamIO *stream = new LocalAssetStream("./light.ent");
	
	//stream->OpenStreamWrite();
	//scene->GetEntityManager().SerializeEntity(e.GetId(), *stream);
	
	//stream->OpenStreamRead();
	//scene->GetEntityManager().LoadEntities(1, *stream);
	//e.Kill();
	//stream->CloseStream();

	scene->GetSystemManager().InitializeSystems();

	Snowfall::GetGameInstance().SetScene(scene);
	Snowfall::GetGameInstance().RegisterUIContext("TestUI", new TestUI);

	delete stream;
}

void SnowfallGame::ReleaseModule()
{
	delete scene;
}