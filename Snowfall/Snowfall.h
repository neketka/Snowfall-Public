#pragma once
#include "ShaderPreprocessor.h"
#include "AssetManager.h"
#include "Scene.h"
#include <map>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <string>

enum class LogType
{
	Message=0, Warning=1, Error=2
};

class Snowfall
{
public:
	Snowfall();
	~Snowfall();
	void StartGame();
	inline ShaderPreprocessor& GetShaderPreprocessor() { return m_preprocessor; }

	static void InitGlobalInstance();
	inline static Snowfall& GetGameInstance() { return *m_gameInstance; }
	inline AssetManager& GetAssetManager() { return m_assetManager; }

	inline float GetFPS() { return m_fps; }

	void Log(LogType type, std::string message);

	inline void SetScene(Scene& scene) { m_scene = &scene; }
	inline Scene& GetScene(Scene& scene) { return *m_scene; }
private:
	float m_fps;
	Scene *m_scene;
	AssetManager m_assetManager;
	ShaderPreprocessor m_preprocessor; // Global preprocessor
	GLFWwindow *m_window;
	static Snowfall *m_gameInstance; // Global game instance
};
