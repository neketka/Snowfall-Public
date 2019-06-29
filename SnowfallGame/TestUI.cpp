#include <Snowfall.h>
#include <UIEvent.h>

#include "TestUI.h"

TestUI::TestUI() : UIContext(800, 600)
{
	m_root = new UIComponent;

	m_root->SetContext(this);

	SetRootComponent(m_root);

	m_root->OnRender.Subscribe(&TestUI::RenderTest, this);
}

bool HoverColor = true;

void TestUI::RenderTest(RenderEventArgs& args)
{
}