#pragma once
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <map>
#include <queue>

#include "export.h"
#include "InputEnums.h"
#include "UIEvent.h"

enum class InputMethod
{
	Key, MouseButton, MouseX, MouseY, ScrollX, ScrollY
};

union InputSource
{
	Key Key;
	MouseButton Button;
};

class MouseMoveEvent
{
public:
	glm::vec2 OldPos;
	glm::vec2 NewPos;
};

class MouseButtonEvent
{
public:
	glm::vec2 CursorPos;
	MouseButton Button;
	bool State;
	bool Repeating;
};

class MouseScrollEvent
{
public:
	glm::vec2 OldPos;
	glm::vec2 NewPos;
};

class KeyEvent
{
public:
	Key Key;
	unsigned ModifierFlag;
	bool State;
	bool Repeating;
};

class TextEvent
{
public:
	char Character;
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
	std::map<std::string, InputAxis> GetAxis() { return m_axes; }
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
	SNOWFALLENGINE_API bool IsMouseLocked();
	SNOWFALLENGINE_API std::string GetClipboardText();
	SNOWFALLENGINE_API void SetClipboardText(std::string text);

	SNOWFALLENGINE_API float GetAxis(std::string axis);
	SNOWFALLENGINE_API void SetProfile(InputProfile profile);

	template<class T>
	std::vector<T>& GetEventQueue();
	SNOWFALLENGINE_API void ClearEventQueues();

private:
	static glm::vec2 m_scrollPos;
	static glm::vec2 m_mousePos;

	bool m_mouseLocked;

	SNOWFALLENGINE_API static std::vector<MouseScrollEvent> m_scrollQueue;
	SNOWFALLENGINE_API static std::vector<KeyEvent> m_keyQueue;
	SNOWFALLENGINE_API static std::vector<TextEvent> m_textQueue;
	SNOWFALLENGINE_API static std::vector<MouseMoveEvent> m_mouseMoveQueue;
	SNOWFALLENGINE_API static std::vector<MouseButtonEvent> m_mouseButtonQueue;

	static void scrollMoved(GLFWwindow *window, double x, double y);
	static void keyPressed(GLFWwindow *window, int key, int scancode, int action, int mods);
	static void characterTyped(GLFWwindow *window, unsigned int codepoint);
	static void cursorMoved(GLFWwindow *window, double xpos, double ypos);
	static void mouseClicked(GLFWwindow* window, int button, int action, int mods);

	InputProfile m_profile;
	GLFWwindow *m_window;
};

template<>
inline std::vector<MouseScrollEvent>& InputManager::GetEventQueue<MouseScrollEvent>()
{
	return m_scrollQueue;
}

template<>
inline std::vector<KeyEvent>& InputManager::GetEventQueue<KeyEvent>()
{
	return m_keyQueue;
}

template<>
inline std::vector<TextEvent>& InputManager::GetEventQueue<TextEvent>()
{
	return m_textQueue;
}

template<>
inline std::vector<MouseMoveEvent>& InputManager::GetEventQueue<MouseMoveEvent>()
{
	return m_mouseMoveQueue;
}

template<>
inline std::vector<MouseButtonEvent>& InputManager::GetEventQueue<MouseButtonEvent>()
{
	return m_mouseButtonQueue;
}