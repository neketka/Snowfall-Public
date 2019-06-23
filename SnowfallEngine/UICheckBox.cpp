#include "stdafx.h"
#include "UICheckBox.h"

UICheckBox::UICheckBox()
{
	MouseDown.Subscribe(&UICheckBox::OnMouseDown, this);
	MouseUp.Subscribe(&UICheckBox::OnMouseUp, this);
	MouseEnter.Subscribe(&UICheckBox::OnEnter, this);
	MouseLeave.Subscribe(&UICheckBox::OnLeave, this);
	MouseClick.Subscribe(&UICheckBox::OnMouseClick, this);
	KeyDown.Subscribe(&UICheckBox::OnKeyDown, this);
	KeyUp.Subscribe(&UICheckBox::OnKeyUp, this);
	OnRender.Subscribe(&UICheckBox::RenderComponent, this);
}

void UICheckBox::SetGrouped(bool grouped)
{
	m_grouped = grouped;
}

bool UICheckBox::GetGrouped()
{
	return m_grouped;
}

void UICheckBox::SetChecked(bool checked)
{
	BoolEventArgs arg;
	arg.OldValue = m_checked;
	arg.NewValue = m_checked = !m_checked;

	m_checked = checked;

	CheckedChanged.Fire(arg);
}

bool UICheckBox::GetChecked()
{
	return m_checked;
}

void UICheckBox::RenderComponent(RenderEventArgs& args)
{
	float borderThickness = 1;
	float inset = 3;

	glm::vec4 borderColor = glm::vec4(0, 0, 0, 1);
	glm::vec4 fillColor = m_hovering ? m_clicking ? glm::vec4(0.7f, 0.7f, 0.7f, 1) : glm::vec4(0.9f, 0.9f, 0.9f, 1)
		: m_clicking ? glm::vec4(0.7f, 0.7f, 0.7f, 1) : glm::vec4(1, 1, 1, 1);
	args.Renderer->RenderRectangle(GetBounds().RelativeRegion, borderColor, fillColor, borderThickness, true);

	Quad2D boxRegion = GetBounds().RelativeRegion.Inset(inset, inset, inset, inset);

	if (m_checked)
		args.Renderer->RenderRectangle(boxRegion, glm::vec4(0, 0, 0, 1));
}

void UICheckBox::OnEnter(MouseMotionEventArgs& args)
{
	m_hovering = true;
}

void UICheckBox::OnLeave(MouseMotionEventArgs& args)
{
	m_hovering = false;
}

void UICheckBox::OnMouseDown(MouseButtonEventArgs& args)
{
	if (args.Primary)
	{
		m_clicking = true;
		Focus();
	}
}

void UICheckBox::OnMouseUp(MouseButtonEventArgs& args)
{
	if (args.Primary)
		m_clicking = false;
}

void UICheckBox::OnMouseClick(MouseButtonEventArgs& args)
{
	if (args.Primary)
	{
		BoolEventArgs arg;
		arg.OldValue = m_checked;
		arg.NewValue = m_checked = !m_checked;
		
		CheckedChanged.Fire(arg);
	}
}

void UICheckBox::OnKeyDown(KeyEventArgs& args)
{
	if (args.Key == Key::KEY_SPACE || args.Key == Key::KEY_ENTER)
		m_clicking = true;
}

void UICheckBox::OnKeyUp(KeyEventArgs& args)
{
	if (args.Key == Key::KEY_SPACE || args.Key == Key::KEY_ENTER)
	{
		m_clicking = false;

		BoolEventArgs arg;
		arg.OldValue = m_checked;
		arg.NewValue = m_checked = !m_checked;

		CheckedChanged.Fire(arg);
	}
}
