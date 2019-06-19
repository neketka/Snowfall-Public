#pragma once

#include "UIComponent.h"
#include "Quad.h"

class InputManager;
class UIRenderer;

class TimedEvent
{
public:
	float TimeoutLength;
	bool Repeating;
};

class UIContext
{
public:
	~UIContext();

	void UpdateContext(UIRenderer& uiRenderer, float time);
	void SetRootComponent(UIComponent *component);
	void SetBounds(IQuad2D bounds);
	void SetDoubleClickTime(float time);
	void SetHoverTime(float time);
	void SetInputManager(InputManager* manager);
	void SetDispatchingEvents(bool dispatching);
	void SetDispatchingTimers(bool dispatching);
	void DispatchFocusChange(UIComponent *component);
	void SetTimer(TimedEvent event);

	IQuad2D GetBounds();
	float GetDoubleClickTime();
	float GetHoverTime();
	bool IsDispatchingEvents();
	bool IsDispatchingTimers();
	UIComponent* GetRootComponent();
private:
	float m_currentTime;
	std::vector<TimedEvent> m_events;
	float m_doubleClickTime;
	float m_hoverTime;
	UIComponent *m_root;
	bool m_dispatchingEvents;
	bool m_dispatchingTimers;
	UIComponent *m_focused;
	InputManager *m_inputManager;
	IQuad2D m_bounds;
};