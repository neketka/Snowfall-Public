#pragma once

#include <UIContext.h>
#include <UIButton.h>
#include <UICheckBox.h>
#include <UIScrollBar.h>
#include <UILabel.h>
#include <UIViewport.h>
#include <UITextBox.h>

class TestUI : public UIContext
{
public:
	TestUI();
private:
	void RenderTest(RenderEventArgs& args);
	void CheckedChanged(BoolEventArgs& args);
	void HorizontalScroll(ScrollBarScrollEventArgs& args);
	void VerticalScroll(ScrollBarScrollEventArgs& args);
	void ClickButton(EventArgs& args);

	UIComponent *m_root;
	UIButton *m_button;
	UICheckBox *m_checkbox;
	UIScrollBar *m_scrollBar;
	UIScrollBar *m_hScrollBar;
	UILabel *m_label;
	UITextBox *m_textbox;
	UIViewport *m_viewport;
};