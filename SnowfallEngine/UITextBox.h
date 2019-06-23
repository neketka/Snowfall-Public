#pragma once
#include "UIComponent.h"

class TextChangedArgs
{
public:
	std::string OldText;
	std::string NewText;
};

class CaretMovedEventArgs
{
public:
	int OldPos;
	int NewPos;
};

class UITextBox : public UIComponent
{
public:
	SNOWFALLENGINE_API UITextBox();

	SNOWFALLENGINE_API void SetText(std::string text);
	SNOWFALLENGINE_API std::string GetText();

	SNOWFALLENGINE_API void SetPlaceholder(std::string text);
	SNOWFALLENGINE_API std::string GetPlaceholder();

	SNOWFALLENGINE_API FontAsset *GetFont();
	SNOWFALLENGINE_API float GetFontSize();
	SNOWFALLENGINE_API glm::vec4 GetFontColor();

	SNOWFALLENGINE_API void SetFont(FontAsset *font, glm::vec4 color, float size);

	SNOWFALLENGINE_API int GetCaretPosition();
	SNOWFALLENGINE_API void SetCaretPosition(int pos, bool selecting=false);

	UIEvent<TextChangedArgs> TextChanged;
	UIEvent<KeyEventArgs> Submit;
private:
	void ScrollOnCaret();

	bool m_selecting;
	int m_selectionAnchor;

	int m_caretPos;
	float m_caretPixelPos;
	float m_scrollOffset;
	std::string m_text;
	std::string m_placeholder;
	
	FontAsset *m_font;
	glm::vec4 m_fontColor;
	float m_fontSize;

	float m_blinkTimeConst;
	float m_lastBlinkTime;
	bool m_showCaret;
	bool m_colorBorder;

	void RenderComponent(RenderEventArgs& args);

	void OnTyping(TextEventArgs& args);
	void OnKeyDown(KeyEventArgs& args);
	void OnMouseDown(MouseButtonEventArgs& args);
	void OnMouseUp(MouseButtonEventArgs& args);
	void OnMouseMove(MouseMotionEventArgs& args);

	void OnMouseEnter(MouseMotionEventArgs& args);
	void OnMouseLeave(MouseMotionEventArgs& args);
	void OnFocused(FocusEventArgs& args);
	void OnLostFocus(FocusEventArgs& args);
};

