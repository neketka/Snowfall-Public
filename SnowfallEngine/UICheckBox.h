#pragma once
#include "UIComponent.h"

class UICheckBox : public UIComponent
{
public:
	SNOWFALLENGINE_API UICheckBox();

	SNOWFALLENGINE_API void SetGrouped(bool grouped);
	SNOWFALLENGINE_API bool GetGrouped();

	SNOWFALLENGINE_API void SetChecked(bool checked);
	SNOWFALLENGINE_API bool GetChecked();
	
	UIEvent<BoolEventArgs> CheckedChanged;
private:
	
	bool m_checked = false;
	bool m_grouped = false;
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