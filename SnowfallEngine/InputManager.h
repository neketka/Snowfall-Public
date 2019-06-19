#pragma once
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <map>

#include "export.h"
#include "InputEnums.h"

enum class InputMethod
{
	Key, MouseButton, MouseX, MouseY, ScrollX, ScrollY
};

union InputSource
{
	Key Key;
	MouseButton Button;
};

class InputManager;

class InputAxis
{
public:
	std::string Name;

	InputMethod PositiveMethod = InputMethod::Key;
	InputSource PositiveSource;

	InputMethod NegativeMethod = InputMethod::Key;
	InputSource NegativeSource;

	float Max = 0;
	float Min = 0;
	float LossPerSec = 0;
	bool Bounded = false;
private:
	friend InputManager;
	float value = 0;
};

class InputProfile
{
public:
	void AddInputAxis(InputAxis axis) { m_axes[axis.Name] = axis; }
	std::map<std::string, InputAxis> GetAxes() { return m_axes; }
private:
	std::map<std::string, InputAxis> m_axes;
};

class InputManager
{
public:
	SNOWFALLENGINE_API InputManager(GLFWwindow *window);

	SNOWFALLENGINE_API void Update(float deltaTime);

	SNOWFALLENGINE_API bool IsKeyDown(Key key);
	SNOWFALLENGINE_API bool IsButtonDown(MouseButton button);
	SNOWFALLENGINE_API glm::vec2 GetScrollPosition();
	SNOWFALLENGINE_API glm::vec2 GetMousePosition();
	SNOWFALLENGINE_API void SetMousePosition(glm::vec2 position);
	SNOWFALLENGINE_API void SetMouseLocked(bool locked);

	SNOWFALLENGINE_API float GetAxis(std::string axis);
	SNOWFALLENGINE_API void SetProfile(InputProfile profile);
private:
	static glm::vec2 m_scrollPos;
	static void scrollMoved(GLFWwindow *window, double x, double y);
	InputProfile m_profile;
	GLFWwindow *m_window;
};

