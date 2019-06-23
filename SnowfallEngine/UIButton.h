#pragma once

#include "UIComponent.h"
#include "FontAsset.h"
#include "export.h"

#include <string>

class UIButton : public UIComponent
{
public:
	SNOWFALLENGINE_API UIButton();

	SNOWFALLENGINE_API void SetText(std::string text);
	SNOWFALLENGINE_API std::string GetText();

	SNOWFALLENGINE_API FontAsset *GetFont();
	SNOWFALLENGINE_API float GetFontSize();
	SNOWFALLENGINE_API glm::vec4 GetFontColor();

	SNOWFALLENGINE_API void SetFont(FontAsset *font, glm::vec4 color, float size);

	UIEvent<EventArgs> Click;

private:
	std::string m_text;

	FontAsset *m_font;
	glm::vec4 m_fontColor;
	float m_fontSize;

	bool m_hovering = false;
	bool m_clicking = false;

	void RenderComponent(RenderEventArgs& args);
	void OnEnter(MouseMotionEventArgs& args);
	void OnLeave(MouseMotionEventArgs& args);
	void OnMouseDown(MouseButtonEventArgs& args);
	void OnMouseUp(MouseButtonEventArgs& args);
	void OnMouseClick(MouseButtonEventArgs& args);
	void OnKeyDown(KeyEventArgs& args);
	void OnKeyUp(KeyEventArgs& args);
};

