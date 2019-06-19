#pragma once
#include <Windows.h>

#include "ShaderPreprocessor.h"
#include "AssetManager.h"
#include "MeshManager.h"
#include "InputManager.h"
#include "Scene.h"
#include "TextRenderer.h"
#include "UIRenderer.h"
#include "Module.h"

#include <map>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <string>
#include <vector>

#include "export.h"

enum class LogType
{
	Message=0, Warning=1, Error=2
};

class LogEntry
{
public:
	LogType Type;
	std::string Message;
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

class ModuleInstance 
{
public:
	HMODULE WindowsHandle;
	Module *ModulePointer;
};

class UIContext;

class Snowfall
{
public:
	SNOWFALLENGINE_API Snowfall(EngineSettings settings);
	SNOWFALLENGINE_API ~Snowfall();
	SNOWFALLENGINE_API void StartGame();
	
	SNOWFALLENGINE_API static void InitGlobalInstance(EngineSettings settings);
	SNOWFALLENGINE_API static Snowfall& GetGameInstance();

	inline AssetManager& GetAssetManager() { return *m_assetManager; }
	inline PrototypeManager& GetPrototypeManager() { return *m_prototypeManager; }
	inline MeshManager& GetMeshManager() { return *m_meshManager; }
	inline ShaderPreprocessor& GetShaderPreprocessor() { return *m_preprocessor; }
	inline InputManager& GetInputManager() { return *m_inputManager; }
	inline TextRenderer& GetTextRenderer() { return *m_textRenderer; }
	inline UIRenderer& GetUIRenderer() { return *m_uiRenderer; }

	inline EngineSettings& GetEngineSettings() { return m_settings; }

	SNOWFALLENGINE_API void CreateViewport(int index);
	SNOWFALLENGINE_API void SetViewportCoefficients(int index, Quad2D scale, Quad2D offset);
	SNOWFALLENGINE_API void LoadModules(std::vector<std::string> paths);
	SNOWFALLENGINE_API IQuad2D GetViewport(int index);

	SNOWFALLENGINE_API void RegisterUIContext(std::string name, UIContext *context);
	SNOWFALLENGINE_API UIContext *GetUIContext(std::string name);

	inline float GetFPS() { return m_fps; }
	inline float GetTime() { return m_time; }

	SNOWFALLENGINE_API void Log(LogType type, std::string message);
	SNOWFALLENGINE_API std::vector<LogEntry>& GetLog();

	inline void SetScene(Scene *scene) { 
		m_scene = scene; 
	}
	inline Scene& GetScene(Scene& scene) { return *m_scene; }
private:
	void Init();
	void SetupDefaultPrototypes();
	void DestroyManagers();

	void GameLoop();
	void Cleanup();

	float m_fps;
	float m_time;
	Scene *m_scene;

	AssetManager *m_assetManager;
	PrototypeManager *m_prototypeManager;
	MeshManager *m_meshManager;
	TextRenderer *m_textRenderer;
	ShaderPreprocessor *m_preprocessor; // Global preprocessor
	InputManager *m_inputManager;
	UIRenderer *m_uiRenderer;

	EngineSettings m_settings;
	GLFWwindow *m_window;
	std::vector<ModuleInstance> m_modules;
	std::vector<LogEntry> m_logFile;
	std::map<std::string, UIContext *> m_uiContexts;

	SNOWFALLENGINE_API static Snowfall *m_gameInstance; // Global game instance
};
