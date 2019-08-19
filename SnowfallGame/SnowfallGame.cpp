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
#include <TerrainAsset.h>
#include <TerrainStreamingSystem.h>
#include <EyeAdaptationSystem.h>

#include "CameraFlyComponent.h"
#include "TestComponent.h"
#include "TestUI.h"

#include <glm/gtc/matrix_transform.hpp>

Scene *scene;

void MakeMaterials()
{
	Sampler sampler[2];

	for (Sampler s : sampler)
	{
		s.SetWrapMode(TextureChannel::S, WrapMode::Repeat);
		s.SetWrapMode(TextureChannel::T, WrapMode::Repeat);
		s.SetWrapMode(TextureChannel::R, WrapMode::Repeat);
	}

	Material material;
	material.MaterialShader = &AssetManager::LocateAssetGlobal<ShaderAsset>("SimplePBRShader");
	material.PerObjectParameterCount = 1;

	Snowfall::GetGameInstance().GetAssetManager().AddAsset(new MaterialAsset("FortMaterial", { sampler[0] },
		{ &AssetManager::LocateAssetGlobal<TextureAsset>("fortnite") }, { SamplerProperty(0, 0, 20) }, material));

	Material materialTerr;
	materialTerr.MaterialShader = &AssetManager::LocateAssetGlobal<ShaderAsset>("SimpleTerrainShader");
	materialTerr.PerObjectParameterCount = 1;

	Snowfall::GetGameInstance().GetAssetManager().AddAsset(new MaterialAsset("FortMaterialTerr", 
		{ sampler[1] },
		{ 
			&AssetManager::LocateAssetGlobal<TextureAsset>("Terrain.Terrain")
		}, 
		{ 
			SamplerProperty(0, 0, 20)
		}, materialTerr));

	Snowfall::GetGameInstance().GetAssetManager().AddAsset(new MaterialAsset("TonemappingMaterial",
		Material(&AssetManager::LocateAssetGlobal<ShaderAsset>("TonemappingPost"))));

	Snowfall::GetGameInstance().GetAssetManager().AddAsset(new MaterialAsset("FxaaMaterial",
		Material(&AssetManager::LocateAssetGlobal<ShaderAsset>("FxaaPost"))));

	Snowfall::GetGameInstance().GetAssetManager().AddAsset(new MaterialAsset("BloomMaterial",
		Material(&AssetManager::LocateAssetGlobal<ShaderAsset>("BloomPost"))));
}

void SnowfallGame::InitializeModule()
{
	Snowfall::GetGameInstance().CreateViewport(0);
	Snowfall::GetGameInstance().SetViewportCoefficients(0, Quad2D(0, 0, 1.f, 1.f), Quad2D(0, 0, 0, 0));

	MakeMaterials();
	
	TerrainAsset *terrain = new TerrainAsset("TestTerrain", 257, 129, 0, 128);

	terrain->SetTerrainMaterial(&AssetManager::LocateAssetGlobal<MaterialAsset>("FortMaterialTerr"));
	terrain->SetStream(Snowfall::GetGameInstance().GetAssetManager().CreateUserDataStream("TestTerrain.trasset"));
	terrain->SetSavable(true);

	Snowfall::GetGameInstance().GetAssetManager().AddAsset(terrain);
	
	scene = new Scene;

	scene->GetSystemManager().AddEnabledSystems({
		"CameraFlySystem", "TransformSystem", "PhysicsRigidBodySystem", "PhysicsWorldSystem", "TerrainStreamingSystem", "MeshRenderingSystem", "ShadowMapRenderSystem",
		"LightSystem", "SkyRenderSystem", "CameraSystem", "EyeAdaptationSystem", "PostProcessRenderSystem", "CameraUIRenderSystem", "CameraViewportRenderSystem" 
	});

	Entity camera = scene->GetEntityManager().CreateEntity({ "TransformComponent", "CameraComponent", "CameraFlyComponent", "CameraUIRenderComponent", "PhysicsRigidBodyComponent", "PhysicsSphereCollisionComponent", "EyeAdaptationComponent" });

	camera.GetComponent<TransformComponent>()->Position = glm::vec3(0, 25, 5);
	camera.GetComponent<CameraComponent>()->ViewportIndex = 0;
	camera.GetComponent<CameraComponent>()->FovY = 90;
	camera.GetComponent<CameraUIRenderComponent>()->ContextName = "TestUI";
	camera.SetName("MainCam");

	camera.GetComponent<CameraComponent>()->PostProcessStack.push_back(&AssetManager::LocateAssetGlobal<MaterialAsset>("FxaaMaterial"));
	camera.GetComponent<CameraComponent>()->PostProcessStack.push_back(&AssetManager::LocateAssetGlobal<MaterialAsset>("BloomMaterial"));
	camera.GetComponent<CameraComponent>()->PostProcessStack.push_back(&AssetManager::LocateAssetGlobal<MaterialAsset>("TonemappingMaterial"));

	camera.GetComponent<EyeAdaptationComponent>()->MinLuma = 0.1f;
	camera.GetComponent<EyeAdaptationComponent>()->MaxLuma = 100;
	camera.GetComponent<EyeAdaptationComponent>()->KeyValue = 0.2f;
	camera.GetComponent<EyeAdaptationComponent>()->ExposureBias = 0;
	camera.GetComponent<EyeAdaptationComponent>()->EaseConstant = 1;

	camera.GetComponent<EyeAdaptationComponent>()->TonemappingMaterial = &AssetManager::LocateAssetGlobal<MaterialAsset>("TonemappingMaterial");
	camera.GetComponent<EyeAdaptationComponent>()->BloomMaterial = &AssetManager::LocateAssetGlobal<MaterialAsset>("BloomMaterial");

	camera.GetComponent<PhysicsRigidBodyComponent>()->ShapeTransform = glm::mat4();
	camera.GetComponent<PhysicsRigidBodyComponent>()->ShapeType = CollisionShapeType::Sphere;
	camera.GetComponent<PhysicsRigidBodyComponent>()->LinearFactor = glm::vec3(0, 0, 0);
	camera.GetComponent<PhysicsRigidBodyComponent>()->AngularFactor = glm::vec3(0, 0, 0);
	camera.GetComponent<PhysicsRigidBodyComponent>()->Mass = 1000.f;
	camera.GetComponent<PhysicsSphereCollisionComponent>()->Radius = 0.75f;

	Entity skybox = scene->GetEntityManager().CreateEntity({ "SkyboxComponent" });
	skybox.GetComponent<SkyboxComponent>()->Cubemap = new TextureAsset("", TextureType::TextureCubemap, TextureInternalFormat::RGBA32F, 256, 256, 1, 1);

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
	e.GetComponent<PhysicsBoxCollisionComponent>()->Size = glm::vec3(2, 2, 2);
	e.SetName("Box");

	Entity light0 = scene->GetEntityManager().CreateEntity({ "TransformComponent", "DirectionalLightComponent" });

	light0.GetComponent<TransformComponent>()->Rotation = glm::vec3(45, 0, 0);
	light0.GetComponent<DirectionalLightComponent>()->Enabled = true;
	light0.GetComponent<DirectionalLightComponent>()->Shadowing = true;
	light0.GetComponent<DirectionalLightComponent>()->Intensity = 3.f;
	light0.GetComponent<DirectionalLightComponent>()->ShadowLayerMask = 0x8000000000000000;
	light0.GetComponent<DirectionalLightComponent>()->Color = glm::vec3(1, 1, 1);

	light0.GetComponent<DirectionalLightComponent>()->CascadeSizes[0] = 40;
	light0.GetComponent<DirectionalLightComponent>()->CascadeSizes[1] = 100;
	light0.GetComponent<DirectionalLightComponent>()->CascadeSizes[2] = 300;
	light0.GetComponent<DirectionalLightComponent>()->CascadeSizes[3] = 400;

	Entity terrainEnt = scene->GetEntityManager().CreateEntity({ "TerrainComponent" });

	terrainEnt.GetComponent<TerrainComponent>()->Terrain = &AssetManager::LocateAssetGlobal<TerrainAsset>("TestTerrain");
	terrainEnt.GetComponent<TerrainComponent>()->StreamingCamera = camera;
	terrainEnt.GetComponent<TerrainComponent>()->MaxStreamDistance = 3;
	terrainEnt.GetComponent<TerrainComponent>()->LayerMask = 0x7FFFFFFFFFFFFFFF;

	scene->GetSystemManager().InitializeSystems();

	Snowfall::GetGameInstance().SetScene(scene);
	Snowfall::GetGameInstance().RegisterUIContext("TestUI", new TestUI);
}

void SnowfallGame::ReleaseModule()
{
	delete scene;
}