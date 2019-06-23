#pragma once
#include "UIComponent.h"

class UILabel : public UIComponent
{
public:
	SNOWFALLENGINE_API UILabel();

	SNOWFALLENGINE_API void SetText(std::string text);
	SNOWFALLENGINE_API std::string GetText();

	SNOWFALLENGINE_API FontAsset *GetFont();
	SNOWFALLENGINE_API float GetFontSize();
	SNOWFALLENGINE_API glm::vec4 GetFontColor();

	SNOWFALLENGINE_API void SetFont(FontAsset *font, glm::vec4 color, float size);

private:
	FontAsset *m_font;
	glm::vec4 m_fontColor;
	float m_fontSize;

	void RenderComponent(RenderEventArgs& args);
	std::string m_text;
};

