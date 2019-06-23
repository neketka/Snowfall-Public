#include "stdafx.h"
#include "UIScrollBar.h"

void UIScrollBar::SetScale(float scale)
{
	m_scale = glm::max(1.f, scale);
}

float UIScrollBar::GetScale()
{
	return m_scale;
}

void UIScrollBar::SetPosition(float pos)
{
	ScrollBarScrollEventArgs e;
	e.OldPosition = m_pos;
	e.NewPosition = m_pos = glm::clamp(pos, 0.f, m_scale - 1);

	Scroll.Fire(e);
}

float UIScrollBar::GetPosition()
{
	return m_pos;
}

void UIScrollBar::SetHorizontal(bool horizontal)
{
	m_horizontal = horizontal;
}

bool UIScrollBar::IsHorizontal()
{
	return m_horizontal;
}

void UIScrollBar::OnMouseUp(MouseButtonEventArgs& args)
{
	if (args.Primary)
	{
		m_scrolling = false;
		m_quickScrolling = false;
	}
}

void UIScrollBar::OnMouseDown(MouseButtonEventArgs& args)
{
	Quad2D reg = GetBounds().RelativeRegion;

	Quad2D pin;
	Quad2D leftOfPin;
	Quad2D rightOfPin;
	if (m_horizontal)
	{
		float offsets = reg.Size.x / m_scale;
		pin = Quad2D(offsets * m_pos + reg.Position.x, reg.Position.y, offsets, reg.Size.y);
		leftOfPin = Quad2D(reg.Position, glm::vec2(m_pos * offsets, reg.Size.y));
		rightOfPin = Quad2D((offsets * m_pos + 1) + reg.Position.x, reg.Position.y, reg.Size.x - (offsets * m_pos + 1), reg.Size.y);
		m_scrollOffset = args.MousePosition.x - pin.Position.x;
	}
	else
	{
		float offsets = reg.Size.y / m_scale;
		pin = Quad2D(reg.Position.x, offsets * m_pos + reg.Position.y, reg.Size.x, offsets);
		rightOfPin = Quad2D(reg.Position.x, (offsets * m_pos + 1) + reg.Position.y, reg.Size.x, reg.Size.y - (offsets * m_pos + 1));
		leftOfPin = Quad2D(reg.Position, glm::vec2(reg.Size.x, m_pos * offsets));
		m_scrollOffset = args.MousePosition.y - pin.Position.y;
	}
	if (args.Primary && !args.Repeating)
	{
		if (pin.Intersects(args.MousePosition))
		{
			m_scrolling = true;
			return;
		}
	}
	if (args.Primary)
	{
		if (leftOfPin.Intersects(args.MousePosition))
		{
			SetPosition(m_pos - 1);
			m_quickScrolling = true;
		}
		else if (rightOfPin.Intersects(args.MousePosition))
		{
			SetPosition(m_pos + 1);
			m_quickScrolling = true;
		}
	}
}

void UIScrollBar::OnMouseMove(MouseMotionEventArgs& args)
{
	if (m_scrolling)
	{
		Quad2D reg = GetBounds().RelativeRegion;
		glm::vec2 diff = args.NewPosition - reg.Position;
		if (m_horizontal)
			SetPosition((diff.x - m_scrollOffset) / (reg.Size.x / m_scale));
		else
			SetPosition((diff.y - m_scrollOffset) / (reg.Size.y / m_scale));
	}
}

void UIScrollBar::RenderComponent(RenderEventArgs& args)
{
	Quad2D reg = GetBounds().RelativeRegion;
	Quad2D pin;
	if (m_horizontal)
	{
		float offsets = reg.Size.x / m_scale;
		pin = Quad2D(offsets * m_pos + reg.Position.x, reg.Position.y, offsets, reg.Size.y);
	}
	else
	{
		float offsets = reg.Size.y / m_scale;
		pin = Quad2D(reg.Position.x, offsets * m_pos + reg.Position.y, reg.Size.x, offsets);
	}

	args.Renderer->RenderRectangle(reg, m_quickScrolling ? glm::vec4(0.7f, 0.7f, 0.7f, 1) : glm::vec4(1));
	args.Renderer->RenderRectangle(pin, m_scrolling ? glm::vec4(0.3f, 0.3f, 0.3f, 1) : glm::vec4(0.5f, 0.5f, 0.5f, 1));
}
