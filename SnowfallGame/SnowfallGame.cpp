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

#include "CameraFlyComponent.h"
#include "TestComponent.h"
#include "TestUI.h"

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

	Snowfall::GetGameInstance().GetAssetManager().AddAsset(new MaterialAsset("ChromaticMaterial",
		Material(&AssetManager::LocateAssetGlobal<ShaderAsset>("ChromaticAbberationPost"))));

	Snowfall::GetGameInstance().GetAssetManager().AddAsset(new MaterialAsset("BloomMaterial",
		Material(&AssetManager::LocateAssetGlobal<ShaderAsset>("BloomPost"))));

	scene = new Scene;

	scene->GetSystemManager().AddEnabledSystems({
		"TransformSystem", "TestSystem", "MeshRenderingSystem", "ShadowMapRenderSystem",
		"LightSystem", "CameraSystem", "PostProcessRenderSystem", "CameraUIRenderSystem",
		"CameraViewportRenderSystem", "CameraFlySystem" 
	});

	Entity camera = scene->GetEntityManager().CreateEntity({ "TransformComponent", "CameraComponent", "CameraFlyComponent", "CameraUIRenderComponent" });
	camera.GetComponent<TransformComponent>()->Position = glm::vec3(0, 5, 5);
	camera.GetComponent<CameraComponent>()->ViewportIndex = 0;
	camera.GetComponent<CameraComponent>()->FovY = 90;
	camera.GetComponent<CameraUIRenderComponent>()->ContextName = "TestUI";

	Entity camera1 = scene->GetEntityManager().CreateEntity({ "TransformComponent", "CameraComponent" });

	camera1.SetName("camera1");
	camera1.GetComponent<TransformComponent>()->Position = glm::vec3(0, 5, 5);
	camera1.GetComponent<CameraComponent>()->ViewportIndex = 1;
	camera1.GetComponent<CameraComponent>()->FovY = 90;

	camera.GetComponent<CameraComponent>()->PostProcessStack.push_back(&AssetManager::LocateAssetGlobal<MaterialAsset>("FxaaMaterial"));
	camera.GetComponent<CameraComponent>()->PostProcessStack.push_back(&AssetManager::LocateAssetGlobal<MaterialAsset>("BloomMaterial"));
	camera.GetComponent<CameraComponent>()->PostProcessStack.push_back(&AssetManager::LocateAssetGlobal<MaterialAsset>("TonemappingMaterial"));

	Entity skybox = scene->GetEntityManager().CreateEntity({ "SkyboxComponent" });
	skybox.GetComponent<SkyboxComponent>()->Cubemap = &AssetManager::LocateAssetGlobal<TextureAsset>("Daylight");

	Entity e = scene->GetEntityManager().CreateEntity({ "TransformComponent", "MeshRenderComponent", "TestComponent" });
	
	e.GetComponent<TransformComponent>()->Position = glm::vec3(1.5f, 3.f, 1.5f);
	e.GetComponent<TransformComponent>()->Scale = glm::vec3(0.5f, 0.5f, 0.5f);
	e.GetComponent<MeshRenderComponent>()->Material = &AssetManager::LocateAssetGlobal<MaterialAsset>("FortMaterial");
	e.GetComponent<MeshRenderComponent>()->Mesh = &AssetManager::LocateAssetGlobal<MeshAsset>("Monkey.Suzanne");
	e.GetComponent<MeshRenderComponent>()->ObjectParameters = { glm::vec4(1.0f, 0.1f, 1, 1) };
	e.GetComponent<MeshRenderComponent>()->BatchingType = BatchingType::Instanced;

	Entity light0 = scene->GetEntityManager().CreateEntity({ "TransformComponent", "LightComponent" });
				  
	light0.GetComponent<TransformComponent>()->Position = glm::vec3(0, 5, 5);
	light0.GetComponent<TransformComponent>()->Rotation = glm::vec3(45, 0, 0);
	light0.GetComponent<LightComponent>()->Enabled = true;
	light0.GetComponent<LightComponent>()->Shadowing = true;
	light0.GetComponent<LightComponent>()->Intensity = 1.f;
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