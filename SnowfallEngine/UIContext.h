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
	SNOWFALLENGINE_API UIContext(int width, int height);
	SNOWFALLENGINE_API ~UIContext();

	SNOWFALLENGINE_API void UpdateContext(UIRenderer& uiRenderer, float time, bool onlyRender);

	SNOWFALLENGINE_API void SetInputManager(InputManager *manager);
	SNOWFALLENGINE_API void SetRootComponent(UIComponent *component);
	SNOWFALLENGINE_API void SetBounds(IQuad2D bounds);

	SNOWFALLENGINE_API void DispatchFocusChange(UIComponent *component);
	SNOWFALLENGINE_API UIComponent *GetFocusedComponent();

	SNOWFALLENGINE_API void SetDoubleClickTime(float time);
	SNOWFALLENGINE_API void SetHoverTime(float time);
	SNOWFALLENGINE_API void SetPrimaryButton(MouseButton button);
	SNOWFALLENGINE_API void SetSecondaryButton(MouseButton button);

	SNOWFALLENGINE_API void SetDispatchingEvents(bool dispatching);

	SNOWFALLENGINE_API IQuad2D GetBounds();
	SNOWFALLENGINE_API float GetDoubleClickTime();
	SNOWFALLENGINE_API float GetHoverTime();
	SNOWFALLENGINE_API bool IsDispatchingEvents();
	SNOWFALLENGINE_API UIComponent* GetRootComponent();
private:
	glm::vec2 RaycastPosition(glm::vec2 pos) { return pos; }

	float m_currentTime;

	bool m_dispatchingEvents;
	bool m_dispatchingTimers;

	float m_doubleClickTime;
	float m_hoverTime;
	MouseButton m_primaryButton;
	MouseButton m_secondaryButton;

	UIComponent *m_focused;
	UIComponent *m_lastMouseHover;

	UIComponent *m_held[9];

	UIComponent *m_root;

	float m_lastHoverTime;
	float m_doubleClickTiming;
	int m_doubleClickCounter;

	InputManager *m_inputManager;
	IQuad2D m_bounds;
};