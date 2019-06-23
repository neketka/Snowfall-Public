#include "stdafx.h"

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
#include "CameraViewportRenderSystem.h"
#include "ShadowMapRenderSystem.h"
#include "PostProcessRenderSystem.h"
#include "CameraUIRenderSystem.h"

#include <iostream>
#include <fstream>
#include <functional>

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
	GameLoop();
	Cleanup();
}

void GLAPIENTRY MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	Snowfall::GetGameInstance().Log(LogType::Warning, "GL Callback: " + std::string(message));
}


void Snowfall::InitGlobalInstance(EngineSettings settings)
{
	m_gameInstance = new Snowfall(settings);
}

Snowfall& Snowfall::GetGameInstance()
{ 
	return *m_gameInstance;
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

typedef Module *(*CreateModule)(void);

void Snowfall::LoadModules(std::vector<std::string> paths)
{
	std::vector<ModuleInstance> modules;
	for (std::string path : paths)
	{
		ModuleInstance inst;
		inst.WindowsHandle = LoadLibraryA(path.c_str());
		if (!inst.WindowsHandle)
		{
			Log(LogType::Error, "Could not load module: " + path);
			return;
		}
		CreateModule fun = reinterpret_cast<CreateModule>(GetProcAddress(inst.WindowsHandle, "CreateModule"));
		if (!fun)
			return;

		inst.ModulePointer = fun();

		if (!inst.ModulePointer)
			return;

		modules.push_back(inst);
		inst.ModulePointer->InitializePrototypes(*m_prototypeManager);
	}

	for (ModuleInstance& inst : modules)
	{
		inst.ModulePointer->InitializeModule();
		Log(LogType::Message, "Loaded module: " + inst.ModulePointer->GetModuleName());
	}

	m_modules.insert(m_modules.begin(), modules.begin(), modules.end());
}

IQuad2D Snowfall::GetViewport(int index)
{
	Quad2D scale = scales[index];
	Quad2D offset = offsets[index];
	int w = 0, h = 0;
	glfwGetWindowSize(m_window, &w, &h);
	return IQuad2D(glm::vec2(w, h) * scale.Position + offset.Position, glm::vec2(w, h) * scale.Size + offset.Size);
}

void Snowfall::RegisterUIContext(std::string name, UIContext *context)
{
	m_uiContexts[name] = context;
}

UIContext *Snowfall::GetUIContext(std::string name)
{
	auto iter = m_uiContexts.find(name);
	if (iter == m_uiContexts.end())
		return nullptr;
	return iter->second;
}

void Snowfall::Log(LogType type, std::string message)
{
	std::string timeStamp = "";
	message = timeStamp + message + "\n";

	std::ofstream stream("./log.txt", std::ofstream::app);
	stream << message;
	stream.close();

#ifdef _DEBUG
	OutputDebugStringA(message.c_str());
#endif
	LogEntry entry;
	entry.Type = type;
	entry.Message = message;

	//m_logFile.push_back(entry);
}

std::vector<LogEntry>& Snowfall::GetLog()
{
	return m_logFile;
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
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	m_assetManager = new AssetManager;
	m_prototypeManager = new PrototypeManager;
	m_meshManager = new MeshManager(m_settings.MaxMeshCommands, m_settings.MaxMeshMemoryBytes / sizeof(RenderVertex));
	m_textRenderer = new TextRenderer(8192);
	m_preprocessor = new ShaderPreprocessor(*m_assetManager);
	m_inputManager = new InputManager(m_window);
	m_uiRenderer = new UIRenderer;

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

	SetupDefaultPrototypes();
}

void Snowfall::SetupDefaultPrototypes()
{
	m_prototypeManager->AddComponentDescription<TransformComponent>();
	m_prototypeManager->AddComponentDescription<MeshRenderComponent>();
	m_prototypeManager->AddComponentDescription<CameraComponent>();
	m_prototypeManager->AddComponentDescription<LightComponent>();
	m_prototypeManager->AddComponentDescription<CameraUIRenderComponent>();
	m_prototypeManager->AddComponentDescription<SkyboxComponent>();

	m_prototypeManager->AddSystemPrototype<CameraSystem>();
	m_prototypeManager->AddSystemPrototype<CameraViewportRenderSystem>();
	m_prototypeManager->AddSystemPrototype<LightSystem>();
	m_prototypeManager->AddSystemPrototype<MeshRenderingSystem>();
	m_prototypeManager->AddSystemPrototype<PostProcessRenderSystem>();
	m_prototypeManager->AddSystemPrototype<ShadowMapRenderSystem>();
	m_prototypeManager->AddSystemPrototype<TransformSystem>();
	m_prototypeManager->AddSystemPrototype<CameraUIRenderSystem>();
}

void Snowfall::DestroyManagers()
{
	delete m_preprocessor;
	delete m_inputManager;
	delete m_textRenderer;
	delete m_uiRenderer;
	delete m_assetManager;
	delete m_prototypeManager;
	delete m_meshManager;
}

void Snowfall::GameLoop()
{
	clock_t lastFrame = clock();

	clock_t fpsTime = 0;
	int fpsCounted = 0;
	const int maxFpsCounts = 60;

	while (!glfwWindowShouldClose(m_window))
	{
		clock_t beginTime = clock();
		float clockDiff = static_cast<float>(beginTime - lastFrame) / CLOCKS_PER_SEC;

		m_inputManager->ClearEventQueues();
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
}

void Snowfall::Cleanup()
{
	for (ModuleInstance inst : m_modules)
	{
		inst.ModulePointer->ReleaseModule();
	}

	DestroyManagers();
	glfwDestroyWindow(m_window);
	glfwTerminate();

	for (ModuleInstance inst : m_modules)
	{
		FreeLibrary(inst.WindowsHandle);
	}

	delete m_gameInstance;
}
