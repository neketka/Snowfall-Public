#include "stdafx.h"
#include "UILabel.h"
#include "TextRenderer.h"

UILabel::UILabel()
{
	m_fontSize = 24;
	m_fontColor = glm::vec4(0, 0, 0, 1);
	m_font = &AssetManager::LocateAssetGlobal<FontAsset>("ArialBasic");
	OnRender.Subscribe(&UILabel::RenderComponent, this);
}

void UILabel::SetText(std::string text)
{
	m_text = text;
}

std::string UILabel::GetText()
{
	return m_text;
}

FontAsset *UILabel::GetFont()
{
	return m_font;
}

float UILabel::GetFontSize()
{
	return m_fontSize;
}

glm::vec4 UILabel::GetFontColor()
{
	return m_fontColor;
}

void UILabel::SetFont(FontAsset *font, glm::vec4 color, float size)
{
	m_font = font;
	m_fontColor = color;
	m_fontSize = size;
}

void UILabel::RenderComponent(RenderEventArgs& args)
{
	float len = m_font->GetTextLength(m_text, m_fontSize);
	float halfHeight = m_fontSize / 2.f;
	float halfWidth = len > GetBounds().RelativeRegion.Size.x ? GetBounds().RelativeRegion.Size.x / 2.f : len / 2.f;

	args.Renderer->RenderText(m_font, GetBounds().RelativeRegion.GetCenter() - glm::vec2(halfWidth, halfHeight), m_fontSize, m_fontColor, m_text);
}