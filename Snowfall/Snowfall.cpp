#include "Snowfall.h"
#include "MeshAsset.h"
#include "TextureAsset.h"
#include <GL\glew.h>
#include <time.h>

Snowfall *Snowfall::m_gameInstance;

Snowfall::Snowfall() : m_preprocessor(m_assetManager)
{
	glfwInit();
	// Initialize window properties
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, true);
	// Create window and OpenGL context
	m_window = glfwCreateWindow(800, 600, "Snowfall", nullptr, nullptr);  
	glfwMakeContextCurrent(m_window);
	glewExperimental = GL_TRUE;
	glewInit();

	TextureUnitManager::Initialize(84);
	ImageUnitManager::Initialize(32);

	m_assetManager.RegisterReader(new ShaderAssetReader);
	m_assetManager.RegisterReader(new MeshAssetReader);
	m_assetManager.RegisterReader(new TextureAssetReader);
	m_assetManager.EnumerateUnpackedFolder(".\\Assets");
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

		m_scene->PerformUpdate(clockDiff);
		m_scene->RenderCameras(clockDiff);

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


void Snowfall::InitGlobalInstance()
{
	m_gameInstance = new Snowfall;
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