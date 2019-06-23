#pragma once
#include "UIComponent.h"

class ScrollBarScrollEventArgs
{
public:
	float OldPosition;
	float NewPosition;
};

class UIScrollBar : public UIComponent
{
public:
	UIScrollBar() : m_pos(0), m_scale(1), m_quickScrolling(false), m_scrolling(false), m_horizontal(false)
	{
		OnRender.Subscribe(&UIScrollBar::RenderComponent, this);
		MouseUp.Subscribe(&UIScrollBar::OnMouseUp, this);
		MouseDown.Subscribe(&UIScrollBar::OnMouseDown, this);
		MouseMove.Subscribe(&UIScrollBar::OnMouseMove, this);
	}

	SNOWFALLENGINE_API void SetScale(float scale);
	SNOWFALLENGINE_API float GetScale();

	SNOWFALLENGINE_API void SetPosition(float pos);
	SNOWFALLENGINE_API float GetPosition();

	SNOWFALLENGINE_API void SetHorizontal(bool horizontal);
	SNOWFALLENGINE_API bool IsHorizontal();

	UIEvent<ScrollBarScrollEventArgs> Scroll;

private:
	SNOWFALLENGINE_API void OnMouseUp(MouseButtonEventArgs& args);
	SNOWFALLENGINE_API void OnMouseDown(MouseButtonEventArgs& args);
	SNOWFALLENGINE_API void OnMouseMove(MouseMotionEventArgs& args);
	SNOWFALLENGINE_API void RenderComponent(RenderEventArgs& args);

	float m_pos;
	float m_scale;

	float m_scrollOffset;

	bool m_quickScrolling;
	bool m_scrolling;
	bool m_horizontal;
};

