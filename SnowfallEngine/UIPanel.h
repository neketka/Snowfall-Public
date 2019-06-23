#pragma once
#include "UIComponent.h"

class UIPanel : public UIComponent
{
public:
	UIPanel() : m_border(true) {
		SetDrawingChildren(false);
		OnRender.Subscribe(&UIPanel::OnRenderA, this);
	}

	SNOWFALLENGINE_API void SetBorderEnabled(bool enabled);
	SNOWFALLENGINE_API bool GetBorderEnabled();
private:
	void OnRenderA(RenderEventArgs& args);
	bool m_border;
};