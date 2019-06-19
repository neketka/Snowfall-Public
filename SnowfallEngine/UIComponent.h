#pragma once
#include <memory>

#include "UIEvent.h"
#include "UIRect.h"
#include "InputManager.h"

class UIContext;
class UIComponent;

class MouseMotionEventArgs
{
public:
	glm::vec2 OldPosition;
	glm::vec2 NewPosition;
};

class MouseButtonEventArgs 
{
public:
	MouseButton Button;
	glm::vec2 Position;
};

class KeyEventArgs
{
public:
	Key Key;
	char Character;
};

class ScrollEventArgs
{
public:
	int OldPosition;
	int NewPosition;
};

class FocusEventArgs
{
public:
	UIComponent *OldFocused;
	UIComponent *NewFocused;
};

class BoundsEventArgs
{
public:
	UIRect OldBounds;
	UIRect NewBounds;
};

class BoolEventArgs
{
public:
	bool OldValue;
	bool NewValue;
};

class RenderEventArgs
{
public:
	UIRenderer *Renderer;
};

class UIComponent
{
public:
	~UIComponent();

	void Focus();
	void Unfocus();
	bool IsFocused();

	void AddComponent(UIComponent *component);
	void RemoveComponent(UIComponent *component);

	void SetContext(UIContext *context);
	UIContext *GetContext();

	void Render(UIRenderer *renderer);
	void Resize(Quad2D oldParent, Quad2D newParent);
	void UpdateFocus(UIComponent *lostFocus, UIComponent *gainFocus);
	void SortByZOrder();

	UIComponent *FindTopmostIntersector(Quad2D quad);

	UIRect GetBounds();
	void SetBounds(UIRect rect);

	void SetZIndex(int index);
	int GetZIndex();

	void SetVisible(bool visible);
	bool IsVisible();

	void SetEnabled(bool enabled);
	bool IsEnabled();

	UIEvent<BoundsEventArgs> BoundsChanged;
	UIEvent<BoolEventArgs> VisibleChanged;
	UIEvent<BoolEventArgs> EnabledChanged;

	UIEvent<RenderEventArgs> OnRender;

	UIEvent<FocusEventArgs> GainFocus;
	UIEvent<FocusEventArgs> LostFocus;

	UIEvent<MouseMotionEventArgs> MouseMove;
	UIEvent<MouseMotionEventArgs> MouseEnter;
	UIEvent<MouseMotionEventArgs> MouseLeave;
	UIEvent<MouseMotionEventArgs> MouseHover;

	UIEvent<MouseButtonEventArgs> MouseDown;
	UIEvent<MouseButtonEventArgs> MouseUp;
	UIEvent<MouseButtonEventArgs> MouseDoubleClick;

	UIEvent<ScrollEventArgs> MouseScroll;

	UIEvent<KeyEventArgs> KeyDown;
	UIEvent<KeyEventArgs> KeyUp;
	UIEvent<KeyEventArgs> KeyPress;

	UIComponent *Parent;
private:
	std::vector<UIComponent *> m_components;
	UIContext *m_context;
	UIRect m_bounds;
	int m_zIndex;
	bool m_focused;
	bool m_visible;
	bool m_enabled;
};
