#pragma once
#include <memory>
#include <string>

#include "UIEvent.h"
#include "UIRect.h"
#include "InputManager.h"
#include "export.h"

class UIContext;
class UIComponent;
class UIRenderer;

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
	glm::vec2 MousePosition;
	bool State;
	bool Repeating;
	bool Primary;
	bool Secondary;
};

class KeyEventArgs
{
public:
	Key Key;
	unsigned ModifierFlag;
	bool State;
	bool Repeating;
};

class TextEventArgs
{
public:
	char Character;
};

class ScrollEventArgs
{
public:
	glm::vec2 OldPosition;
	glm::vec2 NewPosition;
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
	SNOWFALLENGINE_API UIComponent();
	SNOWFALLENGINE_API ~UIComponent();

	SNOWFALLENGINE_API void Focus();
	SNOWFALLENGINE_API void Unfocus();
	SNOWFALLENGINE_API bool IsFocused();

	SNOWFALLENGINE_API void AddComponent(UIComponent *component);
	SNOWFALLENGINE_API void RemoveComponent(UIComponent *component);
	SNOWFALLENGINE_API std::vector<UIComponent *>& GetChildren();

	SNOWFALLENGINE_API void SetContext(UIContext *context);
	SNOWFALLENGINE_API UIContext *GetContext();

	SNOWFALLENGINE_API void Render(UIRenderer *renderer);
	SNOWFALLENGINE_API void RenderChildren(UIRenderer *renderer);
	SNOWFALLENGINE_API void Resize(Quad2D oldParent, Quad2D newParent);
	SNOWFALLENGINE_API void UpdateFocus(UIComponent *lostFocus, UIComponent *gainFocus);
	SNOWFALLENGINE_API void SortByZOrder();

	SNOWFALLENGINE_API UIComponent *FindTopmostIntersector(Quad2D quad);

	template<class T>
	std::vector<T *> FindComponentsByType()
	{
		std::vector<T *> typed;
		for (UIComponent *child : m_components)
		{
			T *t = dynamic_cast<T *>(child);
			if (t)
				typed.push_back(t);
		}
		return typed;
	}

	SNOWFALLENGINE_API UIRect GetBounds();
	SNOWFALLENGINE_API void SetBounds(UIRect rect);

	SNOWFALLENGINE_API void SetZIndex(int index);
	SNOWFALLENGINE_API int GetZIndex();

	SNOWFALLENGINE_API void SetVisible(bool visible);
	SNOWFALLENGINE_API bool IsVisible();

	SNOWFALLENGINE_API void SetEnabled(bool enabled);
	SNOWFALLENGINE_API bool IsEnabled();

	SNOWFALLENGINE_API void SetDrawingChildren(bool drawing);
	SNOWFALLENGINE_API bool IsDrawingChildren();

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
	UIEvent<MouseButtonEventArgs> MouseClick;
	UIEvent<MouseButtonEventArgs> MouseDoubleClick;

	UIEvent<ScrollEventArgs> MouseScroll;

	UIEvent<KeyEventArgs> KeyDown;
	UIEvent<KeyEventArgs> KeyUp;
	UIEvent<TextEventArgs> KeyPress;

	UIComponent *Parent;
private:
	std::vector<UIComponent *> m_components;
	UIContext *m_context;
	UIRect m_bounds;
	int m_zIndex = -1;
	bool m_focused;
	bool m_visible;
	bool m_enabled;
	bool m_drawChildren;
};
