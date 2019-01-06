#include <Windows.h>

#include "Snowfall.h"
#include "TextureAsset.h"
#include "BasicCamera.h"
#include "MeshAsset.h"

class MyEntity : public Entity
{
public:
	DEFINE_PROTOTYPE(MyEntity)
	MyEntity() : Entity("MyEntity", EntityOptions(false, true, false, true, false, true, false, false))
	{
	}

	virtual void OnSceneAddition() override
	{
		material.MaterialShader = &Snowfall::GetGameInstance().GetAssetManager().LocateAsset<ShaderAsset>("SimplePBRShader");
		material.Constants.AddConstant(2, Snowfall::GetGameInstance().GetAssetManager().LocateAsset<TextureAsset>("fortnite").GetTextureObject(), sampler);
		material.PerObjectParameterCount = 1;

		SetMesh(Snowfall::GetGameInstance().GetAssetManager().LocateAsset<MeshAsset>("Cube.Cube_Cube.002").GetMesh());
		SetMaterial(material);

		Scale = glm::vec3(1.0f, 1.0f, 1.0f);
	}

	virtual void OnUpdate(float deltaTime) override
	{
		SetMaterialParameter(0, glm::vec4(Metalness, Roughness, 0, 0));
		Rotation += glm::vec3(0.0f, 0.5f, 0) * deltaTime;
	}

	float Metalness;
	float Roughness;
private:
	Material material;
	Sampler sampler;
};

class MyCamera : public BasicCamera
{
public:
	DEFINE_PROTOTYPE(MyCamera)
	MyCamera()
	{
		SetName("MyCamera");
		SetClipping(0.3f, 1000.f);
		SetAspect(1.33f);
		SetFOV(1.57f);
	}

	virtual void OnSceneAddition() override
	{
		SetRenderTarget(Framebuffer::GetDefault());
	}

	virtual void OnUpdate(float deltaTime) override
	{
		SetRegion(IQuad2D(glm::ivec2(0, 0), Snowfall::GetGameInstance().GetViewportSize()));
	}
};

class MySceneConfig : public SceneConfiguration
{
public:
	MySceneConfig()
	{
		AddEntityPrototype<MyEntity>();
		AddEntityPrototype<MyCamera>();
	}
};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	Snowfall::InitGlobalInstance();

	Scene scene(new MySceneConfig);
	
	MyEntity& e1 = dynamic_cast<MyEntity&>(scene.AddEntity("MyEntity"));
	e1.Position = glm::vec3(-1.5f, 0, -4);
	e1.Metalness = 0;
	e1.Roughness = 0.2f;

	MyEntity& e2 = dynamic_cast<MyEntity&>(scene.AddEntity("MyEntity"));
	e2.Position = glm::vec3(1.5f, 0, -4);
	e2.Metalness = 1;
	e2.Roughness = 0.2f;

	MyCamera& camera = scene.AddEntity<MyCamera>();
	scene.AddCamera(camera);

	Snowfall::GetGameInstance().SetScene(scene);
	Snowfall::GetGameInstance().StartGame();

	return 0;
}

