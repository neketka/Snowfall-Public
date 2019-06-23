#include "stdafx.h"
#include "UITextBox.h"
#include "UIContext.h"
#include "InputEnums.h"
#include "InputManager.h"

UITextBox::UITextBox()
{
	m_fontSize = 24;
	m_fontColor = glm::vec4(0, 0, 0, 1);
	m_font = &AssetManager::LocateAssetGlobal<FontAsset>("ArialBasic");
	m_blinkTimeConst = 0.5f;
	m_selectionAnchor = 0;
	m_selecting = false;

	OnRender.Subscribe(&UITextBox::RenderComponent, this);
	KeyPress.Subscribe(&UITextBox::OnTyping, this);
	KeyDown.Subscribe(&UITextBox::OnKeyDown, this);
	MouseDown.Subscribe(&UITextBox::OnMouseDown, this);
	MouseUp.Subscribe(&UITextBox::OnMouseUp, this);
	MouseMove.Subscribe(&UITextBox::OnMouseMove, this);
	LostFocus.Subscribe(&UITextBox::OnLostFocus, this);
	GainFocus.Subscribe(&UITextBox::OnFocused, this);
	MouseEnter.Subscribe(&UITextBox::OnMouseEnter, this);
	MouseLeave.Subscribe(&UITextBox::OnMouseLeave, this);
}

void UITextBox::SetText(std::string text)
{
	TextChangedArgs args;
	args.OldText = m_text;
	args.NewText = m_text = text;

	if (m_caretPos > text.length())
	{
		m_caretPos = text.length();
		m_caretPixelPos = m_font->GetTextLength(m_text.substr(0, m_caretPos), m_fontSize);
	}

	ScrollOnCaret();

	TextChanged.Fire(args);
}

std::string UITextBox::GetText()
{
	return m_text;
}

void UITextBox::SetPlaceholder(std::string text)
{
	m_placeholder = text;
}

std::string UITextBox::GetPlaceholder()
{
	return m_placeholder;
}

int UITextBox::GetCaretPosition()
{
	return m_caretPos;
}

void UITextBox::SetCaretPosition(int pos, bool selecting)
{
	m_caretPos = glm::clamp(pos, 0, static_cast<int>(m_text.length()));
	m_caretPixelPos = m_font->GetTextLength(m_text.substr(0, m_caretPos), m_fontSize);
	if (!selecting)
		m_selectionAnchor = m_caretPos;
	ScrollOnCaret();
}

void UITextBox::ScrollOnCaret()
{
	float widthThreshold = GetBounds().RelativeRegion.Size.x - 16 + 1;
	float textLength = glm::max(widthThreshold, m_font->GetTextLength(m_text, m_fontSize));

	if (m_caretPixelPos > m_scrollOffset + widthThreshold)
		m_scrollOffset = glm::min(textLength - widthThreshold, m_caretPixelPos - widthThreshold);
	else if (m_caretPixelPos < m_scrollOffset)
		m_scrollOffset = glm::max(0.f, m_caretPixelPos);
}

FontAsset *UITextBox::GetFont()
{
	return m_font;
}

float UITextBox::GetFontSize()
{
	return m_fontSize;
}

glm::vec4 UITextBox::GetFontColor()
{
	return m_fontColor;
}

void UITextBox::SetFont(FontAsset *font, glm::vec4 color, float size)
{
	m_font = font;
	m_fontColor = color;
	m_fontSize = size;
}

void UITextBox::RenderComponent(RenderEventArgs& args)
{
	Quad2D textRegion = GetBounds().RelativeRegion.Inset(8, 8, 4, 4);
	float caretHeight = glm::min(m_fontSize + 4, textRegion.Size.y);

	textRegion.Position.y = (GetBounds().RelativeRegion.Size.y - caretHeight) / 2 + GetBounds().RelativeRegion.Position.y;

	args.Renderer->RenderRectangle(GetBounds().RelativeRegion, m_colorBorder ? glm::vec4(0, 0, 1, 1) : glm::vec4(0, 0, 0, 1), glm::vec4(1, 1, 1, 1), 1, true);

	Quad2D textClip = textRegion;
	textClip.Position.x -= 4;
	textClip.Position.y = GetBounds().RelativeRegion.Position.y;
	textClip.Size.x += 4;
	textClip.Size.y = GetBounds().RelativeRegion.Size.y;

	args.Renderer->PushClip(textClip);

	if (m_text.empty() && !IsFocused())
		args.Renderer->RenderText(m_font, textRegion.Position + glm::vec2(0, 4), m_fontSize, glm::vec4(0.5f, 0.5f, 0.5f, 1.f), m_placeholder);
	else
		args.Renderer->RenderText(m_font, textRegion.Position + glm::vec2(-m_scrollOffset, 4), m_fontSize, m_fontColor, m_text);

	args.Renderer->PopClip();

	if (Snowfall::GetGameInstance().GetTime() - m_lastBlinkTime >= m_blinkTimeConst)
	{
		m_lastBlinkTime = Snowfall::GetGameInstance().GetTime();
		m_showCaret = !m_showCaret;
	}


	if (m_showCaret && IsFocused())
		args.Renderer->RenderRectangle(Quad2D(textRegion.Position + glm::vec2(-1 - m_scrollOffset + m_caretPixelPos, 0), 
			glm::vec2(1, caretHeight)), glm::vec4(0, 0, 0, 1));

	if (m_selectionAnchor != m_caretPos)
	{
		float minPix = m_font->GetTextLength(m_text.substr(0, glm::min(m_caretPos, m_selectionAnchor)), m_fontSize);
		float maxPix = m_font->GetTextLength(m_text.substr(0, glm::max(m_caretPos, m_selectionAnchor)), m_fontSize);

		args.Renderer->RenderRectangle(Quad2D(textRegion.Position + glm::vec2(-1 - m_scrollOffset + minPix, -2.f / 24.f * m_fontSize),
			glm::vec2(maxPix - minPix, caretHeight + 2.f / 24.f * m_fontSize)), glm::vec4(0, 0, 1, 0.3f));
	}
}

void UITextBox::OnTyping(TextEventArgs& args)
{
	if (args.Character < 32)
		return;
	if (m_caretPos == 0)
		SetText(args.Character + m_text);
	else if (m_caretPos == m_text.length())
		SetText(m_text + args.Character);
	else
		SetText(m_text.substr(0, m_caretPos) + args.Character + m_text.substr(m_caretPos));

	SetCaretPosition(m_caretPos + 1);
}

void UITextBox::OnKeyDown(KeyEventArgs& args)
{
	if (args.Key == Key::KEY_BACKSPACE)
	{
		if (m_selectionAnchor != m_caretPos)
		{
			int min = glm::min(m_caretPos, m_selectionAnchor);
			int max = glm::max(m_caretPos, m_selectionAnchor);

			SetCaretPosition(min);
			SetText(m_text.substr(0, min) + (max < m_text.length() ? m_text.substr(max) : ""));
		}
		else if (m_caretPos > 0)
		{
			SetCaretPosition(m_caretPos - 1);
			SetText(m_text.substr(0, m_caretPos) + (m_caretPos + 1 < m_text.length() ? m_text.substr(m_caretPos + 1) : ""));
		}
	}
	else if (args.Key == Key::KEY_DELETE)
	{
		if (m_selectionAnchor != m_caretPos)
		{
			int min = glm::min(m_caretPos, m_selectionAnchor);
			int max = glm::max(m_caretPos, m_selectionAnchor);

			SetCaretPosition(min);
			SetText(m_text.substr(0, min) + (max < m_text.length() ? m_text.substr(max) : ""));
		}
		else if (m_caretPos < m_text.length())
		{
			SetText(m_text.substr(0, m_caretPos) + (m_caretPos + 1 < m_text.length() ? m_text.substr(m_caretPos + 1) : ""));
		}
	}
	else if (args.Key == Key::KEY_RIGHT)
	{
		m_lastBlinkTime = Snowfall::GetGameInstance().GetTime();
		m_showCaret = true;

		if (m_selectionAnchor != m_caretPos && !(args.ModifierFlag & KEYMOD_SHIFT))
			SetCaretPosition(m_selectionAnchor = glm::max(m_caretPos, m_selectionAnchor));
		else if (m_caretPos < m_text.length())
			SetCaretPosition(m_caretPos + 1, args.ModifierFlag & KEYMOD_SHIFT);
	}
	else if (args.Key == Key::KEY_LEFT)
	{
		m_lastBlinkTime = Snowfall::GetGameInstance().GetTime();
		m_showCaret = true;

		if (m_selectionAnchor != m_caretPos && !(args.ModifierFlag & KEYMOD_SHIFT))
			SetCaretPosition(m_selectionAnchor = glm::min(m_caretPos, m_selectionAnchor));
		else if (m_caretPos > 0)
			SetCaretPosition(m_caretPos - 1, args.ModifierFlag & KEYMOD_SHIFT);
	}
	else if (args.Key == Key::KEY_V && (args.ModifierFlag & KEYMOD_CONTROL))
	{
		std::string pasted = Snowfall::GetGameInstance().GetInputManager().GetClipboardText();
		if (m_caretPos == 0)
			SetText(pasted + m_text);
		else if (m_caretPos == m_text.length())
			SetText(m_text + pasted);
		else
			SetText(m_text.substr(0, m_caretPos) + pasted + m_text.substr(m_caretPos));

		SetCaretPosition(m_caretPos + pasted.length());
	}
	else if (args.Key == Key::KEY_C && (args.ModifierFlag & KEYMOD_CONTROL))
	{
		int min = glm::min(m_caretPos, m_selectionAnchor);
		int max = glm::max(m_caretPos, m_selectionAnchor);

		if (max - min == 0)
			return;

		Snowfall::GetGameInstance().GetInputManager().SetClipboardText(m_text.substr(min, max - min));
	}
	else if (args.Key == Key::KEY_X && (args.ModifierFlag & KEYMOD_CONTROL))
	{
		int min = glm::min(m_caretPos, m_selectionAnchor);
		int max = glm::max(m_caretPos, m_selectionAnchor);

		if (max - min == 0)
			return;

		Snowfall::GetGameInstance().GetInputManager().SetClipboardText(m_text.substr(min, max - min));

		SetCaretPosition(min);
		SetText(m_text.substr(0, min) + (max < m_text.length() ? m_text.substr(max) : ""));
	}
	else if (args.Key == Key::KEY_A && (args.ModifierFlag & KEYMOD_CONTROL))
	{
		m_selectionAnchor = 0;
		m_caretPos = m_text.length();
		m_caretPixelPos = m_font->GetTextLength(m_text.substr(0, m_caretPos), m_fontSize);
	}
	else if (args.Key == Key::KEY_HOME)
		SetCaretPosition(0);
	else if (args.Key == Key::KEY_END)
		SetCaretPosition(m_text.length());
}

void UITextBox::OnMouseDown(MouseButtonEventArgs& args)
{
	Focus();

	float outerDiff = args.MousePosition.x - GetBounds().RelativeRegion.Position.x;
	if (outerDiff > GetBounds().RelativeRegion.Size.x || outerDiff < 0)
		return;
	float mouseDiff = glm::clamp(outerDiff - 8, 0.f, GetBounds().RelativeRegion.Position.x 
		+ GetBounds().RelativeRegion.Size.x - 16) + m_scrollOffset;

	m_font->GetClosestSeparator(m_text, m_fontSize, mouseDiff, m_caretPixelPos, m_caretPos);
	m_selectionAnchor = m_caretPos;
	m_selecting = true;
}

void UITextBox::OnMouseUp(MouseButtonEventArgs& args)
{
	m_selecting = false;
}

void UITextBox::OnMouseMove(MouseMotionEventArgs& args)
{
	if (m_selecting)
	{
		float mouseDiff = args.NewPosition.x - GetBounds().RelativeRegion.Position.x - 8 + m_scrollOffset;
		m_font->GetClosestSeparator(m_text, m_fontSize, mouseDiff, m_caretPixelPos, m_caretPos);
		m_caretPos = glm::clamp(m_caretPos, 0, static_cast<int>(m_text.length()));
		ScrollOnCaret();
	}
}

void UITextBox::OnMouseEnter(MouseMotionEventArgs & args)
{
	m_colorBorder = true;
}

void UITextBox::OnMouseLeave(MouseMotionEventArgs & args)
{
	if (!IsFocused())
		m_colorBorder = false;
}

void UITextBox::OnFocused(FocusEventArgs & args)
{
	m_colorBorder = true;
}

void UITextBox::OnLostFocus(FocusEventArgs & args)
{
	m_colorBorder = false;
}
