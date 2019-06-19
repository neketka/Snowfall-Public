#include "stdafx.h"

#include "InputManager.h"

glm::vec2 InputManager::m_scrollPos;

InputManager::InputManager(GLFWwindow *window) : m_window(window)
{
	glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
	glfwSetScrollCallback(window, scrollMoved);
}

void InputManager::Update(float deltaTime)
{
}

bool InputManager::IsKeyDown(Key key)
{
	return glfwGetKey(m_window, static_cast<int>(key)) == GLFW_PRESS;
}

bool InputManager::IsButtonDown(MouseButton button)
{
	return glfwGetMouseButton(m_window, static_cast<int>(button)) == GLFW_PRESS;
}

glm::vec2 InputManager::GetScrollPosition()
{
	return m_scrollPos;
}

glm::vec2 InputManager::GetMousePosition()
{
	double xpos, ypos;
	glfwGetCursorPos(m_window, &xpos, &ypos);
	return glm::vec2(xpos, ypos);
}

void InputManager::SetMousePosition(glm::vec2 position)
{
	glfwSetCursorPos(m_window, position.x, position.y);
}

void InputManager::SetMouseLocked(bool locked)
{
	glfwSetInputMode(m_window, GLFW_CURSOR, locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

float InputManager::GetAxis(std::string axis)
{
	return 0.0f;
}

void InputManager::SetProfile(InputProfile profile)
{
	m_profile = profile;
}

void InputManager::scrollMoved(GLFWwindow *window, double x, double y)
{
	m_scrollPos.x += x;
	m_scrollPos.y += y;
}
