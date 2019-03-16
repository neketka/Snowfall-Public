#include <Windows.h>

#include <Snowfall.h>
#include <TextureAsset.h>
#include <MeshAsset.h>

#include <TransformComponent.h>
#include <MeshComponent.h>
#include <CameraComponent.h>
#include <LightComponent.h>
#include <ShadowMapRenderSystem.h>
#include <SkyboxComponent.h>

#include "CameraFlyComponent.h"
#include "TestComponent.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	EngineSettings settings;

	settings.TextureUnits = 84;
	settings.ImageUnits = 32;
	settings.MaxMeshMemoryBytes = 1000000000;
	settings.MaxMeshCommands = 200000;
	settings.ShadowMapResolution = 512;
	settings.CloseShadowMapResolution = 2048;

	Snowfall::InitGlobalInstance(settings);

	Snowfall::GetGameInstance().CreateViewport(0);
	Snowfall::GetGameInstance().SetViewportCoefficients(0, Quad2D(0, 0, 1.f, 1.f), Quad2D(0, 0, 0, 0));

	Snowfall::GetGameInstance().GetPrototypeManager().AddComponentDescription<CameraFlyComponent>();
	Snowfall::GetGameInstance().GetPrototypeManager().AddComponentDescription<TestComponent>();

	Sampler sampler;

	Material material;
	material.MaterialShader = &AssetManager::LocateAssetGlobal<ShaderAsset>("SimplePBRShader");
	material.PerObjectParameterCount = 1;

	Snowfall::GetGameInstance().GetAssetManager().AddAsset(new MaterialAsset("FortMaterial", { sampler }, 
		{ &AssetManager::LocateAssetGlobal<TextureAsset>("fortnite") }, { SamplerProperty(0, 0, 15) }, material));

	Scene scene;

	scene.GetSystemManager().AddSystem(new TransformSystem);
	scene.GetSystemManager().AddSystem(new TestSystem);
	scene.GetSystemManager().AddSystem(new MeshRenderingSystem);
	scene.GetSystemManager().AddSystem(new ShadowMapRenderSystem);
	scene.GetSystemManager().AddSystem(new LightSystem);
	scene.GetSystemManager().AddSystem(new CameraSystem);
	scene.GetSystemManager().AddSystem(new CameraFlySystem);

	scene.GetSystemManager().GetSystem("CameraSystem")->SetEnabled(true);
	scene.GetSystemManager().GetSystem("TransformSystem")->SetEnabled(true);
	scene.GetSystemManager().GetSystem("TestSystem")->SetEnabled(true);
	scene.GetSystemManager().GetSystem("MeshRenderingSystem")->SetEnabled(true);
	scene.GetSystemManager().GetSystem("CameraFlySystem")->SetEnabled(true);
	scene.GetSystemManager().GetSystem("ShadowMapRenderSystem")->SetEnabled(true);
	scene.GetSystemManager().GetSystem("LightSystem")->SetEnabled(true);

	Entity camera = scene.GetEntityManager().CreateEntity({ "TransformComponent", "CameraComponent", "CameraFlyComponent" });
	camera.GetComponent<CameraComponent>()->ViewportIndex = 0;
	camera.GetComponent<CameraComponent>()->FovY = 90;
	camera.GetComponent<TransformComponent>()->Position = glm::vec3(0, 5, 5);

	Entity skybox = scene.GetEntityManager().CreateEntity({ "SkyboxComponent" });
	skybox.GetComponent<SkyboxComponent>()->Cubemap = &AssetManager::LocateAssetGlobal<TextureAsset>("Daylight");

	for (int i = 0; i != 16; ++i)
	{
		Entity e = scene.GetEntityManager().CreateEntity({ "TransformComponent", "MeshRenderComponent"});

		float rnd1 = std::floor(i / 4);
		float rnd2 = i % 4;

		e.GetComponent<TransformComponent>()->Position = glm::vec3(rnd1, 0, rnd2);
		e.GetComponent<TransformComponent>()->Scale = glm::vec3(0.5f, 0.5f, 0.5f);
		e.GetComponent<MeshRenderComponent>()->Material = &AssetManager::LocateAssetGlobal<MaterialAsset>("FortMaterial");
		e.GetComponent<MeshRenderComponent>()->Mesh = &AssetManager::LocateAssetGlobal<MeshAsset>("Cube.Cube_Cube.002");
		e.GetComponent<MeshRenderComponent>()->ObjectParameters = { glm::vec4(0.f, 0.7f, 1, 1) };
		e.GetComponent<MeshRenderComponent>()->BatchingType = BatchingType::Instanced;
	}

	Entity e = scene.GetEntityManager().CreateEntity({ "TransformComponent", "MeshRenderComponent", "TestComponent" });

	e.GetComponent<TransformComponent>()->Position = glm::vec3(1.5f, 1.2f, 1.5f);
	e.GetComponent<TransformComponent>()->Scale = glm::vec3(0.5f, 0.5f, 0.5f);
	e.GetComponent<MeshRenderComponent>()->Material = &AssetManager::LocateAssetGlobal<MaterialAsset>("FortMaterial");
	e.GetComponent<MeshRenderComponent>()->Mesh = &AssetManager::LocateAssetGlobal<MeshAsset>("Monkey.Suzanne");
	e.GetComponent<MeshRenderComponent>()->ObjectParameters = { glm::vec4(0.f, 0.7f, 1, 1) };
	e.GetComponent<MeshRenderComponent>()->BatchingType = BatchingType::Instanced;

	Entity e1 = scene.GetEntityManager().CreateEntity({ "TransformComponent", "MeshRenderComponent" });

	e1.GetComponent<TransformComponent>()->Position = glm::vec3(3.f, 1, 2.f);
	e1.GetComponent<TransformComponent>()->Scale = glm::vec3(0.5f, 0.5f, 0.5f);
	e1.GetComponent<MeshRenderComponent>()->Material = &AssetManager::LocateAssetGlobal<MaterialAsset>("FortMaterial");
	e1.GetComponent<MeshRenderComponent>()->Mesh = &AssetManager::LocateAssetGlobal<MeshAsset>("Cube.Cube_Cube.002");
	e1.GetComponent<MeshRenderComponent>()->ObjectParameters = { glm::vec4(0.f, 0.7f, 1, 1) };
	e1.GetComponent<MeshRenderComponent>()->BatchingType = BatchingType::Instanced;
	
	Entity light = scene.GetEntityManager().CreateEntity({ "TransformComponent", "LightComponent" });

	light.GetComponent<TransformComponent>()->Position = glm::vec3(1.5f, 3, 1);
	light.GetComponent<TransformComponent>()->Rotation = glm::vec3(35, 180, 0);
	light.GetComponent<LightComponent>()->Enabled = true;
	light.GetComponent<LightComponent>()->Shadowing = true;
	light.GetComponent<LightComponent>()->Type = LightType::Spot;
	light.GetComponent<LightComponent>()->Color = glm::vec3(1, 1, 1);
	light.GetComponent<LightComponent>()->InnerCutoff = 35.5f;
	light.GetComponent<LightComponent>()->OuterCutoff = 36;
	light.GetComponent<LightComponent>()->Intensity = 5.f;
	light.GetComponent<LightComponent>()->Range = 5.f;
	light.GetComponent<LightComponent>()->RangeCutoff = 0.005f;

	Entity light0 = scene.GetEntityManager().CreateEntity({ "TransformComponent", "LightComponent" });

	light0.GetComponent<TransformComponent>()->Position = glm::vec3(0, 5, 5);
	light0.GetComponent<TransformComponent>()->Rotation = glm::vec3(45, 0, 0);
	light0.GetComponent<LightComponent>()->Enabled = true;
	light0.GetComponent<LightComponent>()->Shadowing = true;
	light0.GetComponent<LightComponent>()->Intensity = 3.f;
	light0.GetComponent<LightComponent>()->Type = LightType::Directional;
	light0.GetComponent<LightComponent>()->Color = glm::vec3(1, 1, 1);

	scene.GetSystemManager().InitializeSystems();

	Snowfall::GetGameInstance().SetScene(scene);
	Snowfall::GetGameInstance().StartGame();

	return 0;
}

