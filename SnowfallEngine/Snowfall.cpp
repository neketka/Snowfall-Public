#include "Snowfall.h"
#include "MeshAsset.h"
#include "TextureAsset.h"
#include "MaterialAsset.h"
#include <GL\glew.h>
#include <time.h>

#include "MeshComponent.h"
#include "TransformComponent.h"
#include "CameraComponent.h"

Snowfall *Snowfall::m_gameInstance;

Snowfall::Snowfall(EngineSettings settings)
{
	m_settings = settings;
	Init();
}

Snowfall::~Snowfall()
{
}

void Snowfall::StartGame()
{
	glfwShowWindow(m_window);
	clock_t lastFrame = clock();

	clock_t fpsTime = 0;
	int fpsCounted = 0;
	const int maxFpsCounts = 60;

	while (!glfwWindowShouldClose(m_window))
	{
		clock_t beginTime = clock();
		float clockDiff = static_cast<float>(beginTime - lastFrame) / CLOCKS_PER_SEC;

		glfwPollEvents(); // Check for user events

		m_meshManager->ClearData();
		m_scene->Update(clockDiff);

		glfwSwapBuffers(m_window);

		clock_t endTime = clock();

		++fpsCounted;
		fpsTime += endTime - beginTime;
		if (fpsCounted >= maxFpsCounts)
			m_fps = static_cast<float>(CLOCKS_PER_SEC / fpsTime) * fpsCounted;

		lastFrame = beginTime;
	}

	glfwDestroyWindow(m_window);
	glfwTerminate();
	delete m_gameInstance;
}


void Snowfall::InitGlobalInstance(EngineSettings settings)
{
	m_gameInstance = new Snowfall(settings);
}

glm::ivec2 Snowfall::GetViewportSize()
{
	int w = 0, h = 0;
	glfwGetWindowSize(m_window, &w, &h);
	return glm::ivec2(w, h);
}

void Snowfall::Log(LogType type, std::string message)
{
}

void Snowfall::Init()
{
	glfwInit();
	// Initialize window properties
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, true);
	// Create window and OpenGL context
	m_window = glfwCreateWindow(800, 600, "Snowfall", nullptr, nullptr);
	glfwMakeContextCurrent(m_window);
	glewExperimental = GL_TRUE;
	glewInit();

	m_assetManager = new AssetManager;
	m_prototypeManager = new PrototypeManager;
	m_meshManager = new MeshManager(m_settings.MaxMeshCommands, m_settings.MaxMeshMemoryBytes / (sizeof(RenderVertex) * 2.1f));
	m_preprocessor = new ShaderPreprocessor(*m_assetManager);

	TextureUnitManager::Initialize(m_settings.TextureUnits);
	ImageUnitManager::Initialize(m_settings.ImageUnits);

	m_assetManager->RegisterReader(new ShaderAssetReader);
	m_assetManager->RegisterReader(new MeshAssetReader);
	m_assetManager->RegisterReader(new TextureAssetReader);
	m_assetManager->RegisterReader(new MaterialAssetReader);

	m_assetManager->EnumerateUnpackedFolder(".\\Assets");

	SetupDefaultPrototypes();
}

void Snowfall::SetupDefaultPrototypes()
{
	m_prototypeManager->AddComponentDescription<TransformComponent>();
	m_prototypeManager->AddComponentDescription<MeshRenderComponent>();
	m_prototypeManager->AddComponentDescription<CameraComponent>();
}