#include "Snowfall.h"
#include "MeshAsset.h"
#include "TextureAsset.h"
#include "MaterialAsset.h"
#include "FontAsset.h"

#include <GL\glew.h>
#include <time.h>

#include "MeshComponent.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "LightComponent.h"
#include "SkyboxComponent.h"

#include <iostream>
#include <fstream>
#include <Windows.h>

Snowfall *Snowfall::m_gameInstance;

Snowfall::Snowfall(EngineSettings settings)
{
	m_settings = settings;
	Init();
}

Snowfall::~Snowfall()
{
	delete m_assetManager;
	delete m_prototypeManager;
	delete m_meshManager;
	delete m_preprocessor;
	delete m_inputManager;
	delete m_textRenderer;
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
		{
			m_fps = CLOCKS_PER_SEC / static_cast<float>(fpsTime) * fpsCounted;
			//Log(LogType::Message, std::to_string(m_fps));
			fpsTime = 0;
			fpsCounted = 0;
		}

		lastFrame = beginTime;
		m_time += clockDiff;
	}

	glfwDestroyWindow(m_window);
	glfwTerminate();
	delete m_gameInstance;
}


void Snowfall::InitGlobalInstance(EngineSettings settings)
{
	m_gameInstance = new Snowfall(settings);
}

std::map<int, Quad2D> scales;
std::map<int, Quad2D> offsets;

void Snowfall::CreateViewport(int index)
{
	scales[index] = Quad2D(0, 0, 1, 1);
	offsets[index] = Quad2D(0, 0, 0, 0);
}

void Snowfall::SetViewportCoefficients(int index, Quad2D scale, Quad2D offset)
{
	scales[index] = scale;
	offsets[index] = offset;
}

void Snowfall::LoadModule(std::string path)
{
}

IQuad2D Snowfall::GetViewport(int index)
{
	Quad2D scale = scales[index];
	Quad2D offset = offsets[index];
	int w = 0, h = 0;
	glfwGetWindowSize(m_window, &w, &h);
	return IQuad2D(glm::vec2(w, h) * scale.Position + offset.Position, glm::vec2(w, h) * scale.Size + offset.Size);
}

void Snowfall::Log(LogType type, std::string message)
{
	message += "\n";
	std::cout << message << std::endl;
	OutputDebugStringA(message.c_str());
}

void Snowfall::Init()
{
	glfwInit();
	// Initialize window properties
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, true);
	glfwSwapInterval(0);
	// Create window and OpenGL context

	m_window = glfwCreateWindow(800, 600, "Snowfall", nullptr, nullptr);
	glfwMakeContextCurrent(m_window);
	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	m_assetManager = new AssetManager;
	m_prototypeManager = new PrototypeManager;
	m_meshManager = new MeshManager(m_settings.MaxMeshCommands, m_settings.MaxMeshMemoryBytes / sizeof(RenderVertex));
	m_textRenderer = new TextRenderer(8192);
	m_preprocessor = new ShaderPreprocessor(*m_assetManager);
	m_inputManager = new InputManager(m_window);

	TextureUnitManager::Initialize(m_settings.TextureUnits);
	ImageUnitManager::Initialize(m_settings.ImageUnits);

	m_assetManager->RegisterReader(new ShaderAssetReader);
	m_assetManager->RegisterReader(new MeshAssetReader);
	m_assetManager->RegisterReader(new TextureAssetReader);
	m_assetManager->RegisterReader(new MaterialAssetReader);
	m_assetManager->RegisterReader(new RenderTargetAssetReader);
	m_assetManager->RegisterReader(new FontAssetReader);

	m_assetManager->AddAsset(new MeshAsset("FullScreenQuad", Mesh({
		RenderVertex(glm::vec3(-1, -1, 1)),
		RenderVertex(glm::vec3(1, -1, 1)),
		RenderVertex(glm::vec3(1, 1, 1)),
		RenderVertex(glm::vec3(-1, 1, 1)),
	}, {
		0, 1, 2, 2, 3, 0
	})));

	m_assetManager->EnumerateUnpackedFolder(".\\Assets");

	SetupDefaultPrototypes();
}

void Snowfall::SetupDefaultPrototypes()
{
	m_prototypeManager->AddComponentDescription<TransformComponent>();
	m_prototypeManager->AddComponentDescription<MeshRenderComponent>();
	m_prototypeManager->AddComponentDescription<CameraComponent>();
	m_prototypeManager->AddComponentDescription<LightComponent>();
	m_prototypeManager->AddComponentDescription<SkyboxComponent>();
}
