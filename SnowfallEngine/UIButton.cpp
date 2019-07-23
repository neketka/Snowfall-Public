#include "stdafx.h"
#include "UIButton.h"

UIButton::UIButton()
{
	m_fontSize = 24;
	m_fontColor = glm::vec4(0, 0, 0, 1);
	m_font = &AssetManager::LocateAssetGlobal<FontAsset>("ArialBasic");

	MouseDown.Subscribe(&UIButton::OnMouseDown, this);
	MouseUp.Subscribe(&UIButton::OnMouseUp, this);
	MouseEnter.Subscribe(&UIButton::OnEnter, this);
	MouseLeave.Subscribe(&UIButton::OnLeave, this);
	MouseClick.Subscribe(&UIButton::OnMouseClick, this);
	KeyUp.Subscribe(&UIButton::OnKeyUp, this);
	KeyDown.Subscribe(&UIButton::OnKeyDown, this);

	OnRender.Subscribe(&UIButton::RenderComponent, this);
}

void UIButton::SetText(std::string text)
{
	m_text = text;
}

std::string UIButton::GetText()
{
	return m_text;
}

FontAsset *UIButton::GetFont()
{
	return m_font;
}

float UIButton::GetFontSize()
{
	return m_fontSize;
}

glm::vec4 UIButton::GetFontColor()
{
	return m_fontColor;
}

void UIButton::SetFont(FontAsset *font, glm::vec4 color, float size)
{
	m_font = font;
	m_fontColor = color;
	m_fontSize = size;
}

void UIButton::RenderComponent(RenderEventArgs& args)
{
	float borderThickness = 1;

	glm::vec4 borderColor = glm::vec4(0, 0, 0, 1);
	glm::vec4 fillColor = m_hovering ? m_clicking ? glm::vec4(0.7f, 0.7f, 0.7f, 1) : glm::vec4(0.9f, 0.9f, 0.9f, 1) 
		: m_clicking ? glm::vec4(0.7f, 0.7f, 0.7f, 1) : glm::vec4(1, 1, 1, 1);
	args.Renderer->RenderRectangle(GetBounds().RelativeRegion, borderColor, fillColor, borderThickness, true);

	float halfHeight = m_fontSize / 2.f - 2;
	float halfWidth = m_font->GetTextLength(m_text, m_fontSize) / 2.f;

	args.Renderer->RenderText(m_font, GetBounds().RelativeRegion.GetCenter() - glm::vec2(halfWidth, halfHeight), m_fontSize, m_fontColor, m_text);
}

void UIButton::OnEnter(MouseMotionEventArgs& args)
{
	m_hovering = true;
}

void UIButton::OnLeave(MouseMotionEventArgs& args)
{
	m_hovering = false;
}

void UIButton::OnMouseDown(MouseButtonEventArgs& args)
{
	if (args.Primary)
	{
		m_clicking = true;
		Focus();
	}
}

void UIButton::OnMouseUp(MouseButtonEventArgs& args)
{
	if (args.Primary)
		m_clicking = false;
}

void UIButton::OnMouseClick(MouseButtonEventArgs& args)
{
	if (args.Primary)
	{
		EventArgs arg;
		Click.Fire(arg);
	}
}

void UIButton::OnKeyDown(KeyEventArgs& args)
{
	if (args.Key == Key::KEY_SPACE || args.Key == Key::KEY_ENTER)
		m_clicking = true;
}

void UIButton::OnKeyUp(KeyEventArgs& args)
{
	if (args.Key == Key::KEY_SPACE || args.Key == Key::KEY_ENTER)
	{
		m_clicking = false;

		EventArgs arg;
		Click.Fire(arg);
	}
}
