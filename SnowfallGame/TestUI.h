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

	UIComponent *m_root;
};