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

	InputMethod PositiveMethod;
	InputSource PositiveSource;

	InputMethod NegativeMethod;
	InputSource NegativeSource;

	float Max;
	float Min;
	float LossPerSec;
	bool Bounded;
private:
	friend InputManager;
	float value;
};

class InputProfile
{
public:
	void AddInputAxis(InputAxis axis) { m_axes[axis.Name] = axis; }
	std::map<std::string, InputAxis> GetAxes() { return m_axes; }
private:
	std::map<std::string, InputAxis> m_axes;
};

class SNOWFALLENGINE_API InputManager
{
public:
	InputManager(GLFWwindow *window);

	void Update(float deltaTime);

	bool IsKeyDown(Key key);
	bool IsButtonDown(MouseButton button);
	glm::vec2 GetScrollPosition();
	glm::vec2 GetMousePosition();
	void SetMousePosition(glm::vec2 position);
	void SetMouseLocked(bool locked);

	float GetAxis(std::string axis);
	void SetProfile(InputProfile profile);
private:
	static glm::vec2 m_scrollPos;
	static void scrollMoved(GLFWwindow *window, double x, double y);
	InputProfile m_profile;
	GLFWwindow *m_window;
};

