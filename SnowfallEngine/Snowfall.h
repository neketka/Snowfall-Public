#pragma once
#include "ShaderPreprocessor.h"
#include "AssetManager.h"
#include "MeshManager.h"
#include "InputManager.h"
#include "Scene.h"

#include <map>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <string>

#include "export.h"

enum class LogType
{
	Message=0, Warning=1, Error=2
};

class EngineSettings
{
public:
	int MaxComponentPoolMemory;
	int ComponentPoolChunks;
	int MaxTextureMemory;

	int ShadowMapResolution;
	int CloseShadowMapResolution;

	int MaxMeshMemoryBytes;
	int MaxMeshCommands;
	int TextureUnits;
	int ImageUnits;
};

class SNOWFALLENGINE_API Snowfall
{
public:
	Snowfall(EngineSettings settings);
	~Snowfall();
	void StartGame();
	
	static void InitGlobalInstance(EngineSettings settings);
	inline static Snowfall& GetGameInstance() { return *m_gameInstance; }

	inline AssetManager& GetAssetManager() { return *m_assetManager; }
	inline PrototypeManager& GetPrototypeManager() { return *m_prototypeManager; }
	inline MeshManager& GetMeshManager() { return *m_meshManager; }
	inline ShaderPreprocessor& GetShaderPreprocessor() { return *m_preprocessor; }
	inline InputManager& GetInputManager() { return *m_inputManager; }
	inline EngineSettings& GetEngineSettings() { return m_settings; }

	glm::ivec2 GetViewportSize();

	inline float GetFPS() { return m_fps; }

	void Log(LogType type, std::string message);

	inline void SetScene(Scene& scene) { m_scene = &scene; }
	inline Scene& GetScene(Scene& scene) { return *m_scene; }
private:
	void Init();
	void SetupDefaultPrototypes();
	float m_fps;
	Scene *m_scene;
	AssetManager *m_assetManager;
	PrototypeManager *m_prototypeManager;
	MeshManager *m_meshManager;
	ShaderPreprocessor *m_preprocessor; // Global preprocessor
	EngineSettings m_settings;
	InputManager *m_inputManager;
	GLFWwindow *m_window;
	static Snowfall *m_gameInstance; // Global game instance
};
