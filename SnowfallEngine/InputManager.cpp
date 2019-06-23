#include "stdafx.h"

#include "InputManager.h"

glm::vec2 InputManager::m_scrollPos;
glm::vec2 InputManager::m_mousePos;

std::vector<MouseScrollEvent> InputManager::m_scrollQueue;
std::vector<KeyEvent> InputManager::m_keyQueue;
std::vector<TextEvent> InputManager::m_textQueue;
std::vector<MouseMoveEvent> InputManager::m_mouseMoveQueue;
std::vector<MouseButtonEvent> InputManager::m_mouseButtonQueue;

InputManager::InputManager(GLFWwindow *window) : m_window(window), m_mouseLocked(false)
{
	glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);

	glfwSetScrollCallback(window, scrollMoved);
	glfwSetCharCallback(window, characterTyped);
	glfwSetCursorPosCallback(window, cursorMoved);
	glfwSetMouseButtonCallback(window, mouseClicked);
	glfwSetKeyCallback(window, keyPressed);
}

void InputManager::Update(float deltaTime)
{
}

bool InputManager::IsKeyDown(Key key)
{
	return glfwGetKey(m_window, static_cast<int>(key)) != GLFW_RELEASE;
}

bool InputManager::IsButtonDown(MouseButton button)
{
	return glfwGetMouseButton(m_window, static_cast<int>(button)) != GLFW_RELEASE;
}

glm::vec2 InputManager::GetScrollPosition()
{
	return m_scrollPos;
}

glm::vec2 InputManager::GetMousePosition()
{
	double xpos, ypos;
	glfwGetCursorPos(m_window, &xpos, &ypos);
	int h;
	glfwGetWindowSize(m_window, nullptr, &h);
	return glm::vec2(xpos, h - ypos);
}

void InputManager::SetMousePosition(glm::vec2 position)
{
	int h;
	glfwGetWindowSize(m_window, nullptr, &h);
	glfwSetCursorPos(m_window, position.x, h - position.y);
}

void InputManager::SetMouseLocked(bool locked)
{
	glfwSetInputMode(m_window, GLFW_CURSOR, locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

bool InputManager::IsMouseLocked()
{
	return m_mouseLocked;
}

std::string InputManager::GetClipboardText()
{
	return std::string(glfwGetClipboardString(m_window));
}

void InputManager::SetClipboardText(std::string text)
{
	glfwSetClipboardString(m_window, text.c_str());
}

float InputManager::GetAxis(std::string axis)
{
	return 0.0f;
}

void InputManager::SetProfile(InputProfile profile)
{
	m_profile = profile;
}

void InputManager::ClearEventQueues()
{
	m_scrollQueue.clear();
	m_keyQueue.clear();
	m_scrollQueue.clear();
	m_textQueue.clear();
	m_mouseMoveQueue.clear();
	m_mouseButtonQueue.clear();
}

void InputManager::scrollMoved(GLFWwindow *window, double x, double y)
{
	MouseScrollEvent e;
	e.OldPos = m_scrollPos;

	m_scrollPos.x += x;
	m_scrollPos.y += y;

	e.NewPos = m_scrollPos;

	m_scrollQueue.push_back(e);
}

void InputManager::keyPressed(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	KeyEvent e;
	e.Key = static_cast<Key>(key);
	e.State = action != GLFW_RELEASE;
	e.Repeating = action == GLFW_REPEAT;
	e.ModifierFlag = mods;
	m_keyQueue.push_back(e);
}

void InputManager::characterTyped(GLFWwindow *window, unsigned int codepoint)
{
	if (codepoint > 255)
		return;
	TextEvent e;
	e.Character = static_cast<char>(codepoint);
	m_textQueue.push_back(e);
}

void InputManager::cursorMoved(GLFWwindow *window, double xpos, double ypos)
{
	int h;
	MouseMoveEvent e;
	glfwGetWindowSize(window, nullptr, &h);
	e.OldPos = m_mousePos;
	e.NewPos = m_mousePos = glm::vec2(xpos, h - ypos);
	m_mouseMoveQueue.push_back(e);
}

void InputManager::mouseClicked(GLFWwindow *window, int button, int action, int mods)
{
	MouseButtonEvent e;
	e.CursorPos = m_mousePos;
	e.State = action != GLFW_RELEASE;
	e.Repeating = action == GLFW_REPEAT;
	e.Button = static_cast<MouseButton>(button);
	m_mouseButtonQueue.push_back(e);
}
