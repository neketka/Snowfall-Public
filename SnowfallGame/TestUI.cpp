#include <Snowfall.h>
#include <UIEvent.h>

#include "TestUI.h"

TestUI::TestUI() : UIContext(800, 600)
{
	m_root = new UIComponent;

	m_button = new UIButton;
	m_checkbox = new UICheckBox;
	m_scrollBar = new UIScrollBar;
	m_hScrollBar = new UIScrollBar;
	m_label = new UILabel;
	m_textbox = new UITextBox;
	m_viewport = new UIViewport;

	m_root->SetContext(this);

	m_root->SetBounds(UIRect(Quad2D(0, 0, 800, 600), glm::vec2(0, 0), glm::vec2(800, 600)));
	m_button->SetBounds(UIRect(Quad2D(200, 200, 200, 50), glm::vec2(200, 200), glm::vec2(200, 200)));
	m_checkbox->SetBounds(UIRect(Quad2D(100, 100, 15, 15), glm::vec2(100, 100), glm::vec2(100, 100)));
	m_scrollBar->SetBounds(UIRect(Quad2D(50, 100, 20, 200), glm::vec2(50, 100), glm::vec2(50, 100)));
	m_hScrollBar->SetBounds(UIRect(Quad2D(50, 50, 200, 20), glm::vec2(50, 50), glm::vec2(50, 50)));
	m_label->SetBounds(UIRect(Quad2D(20, 20, 150, 25), glm::vec2(20, 20), glm::vec2(20, 20))); 
	m_viewport->SetBounds(UIRect(Quad2D(300, 0, 200, 200), glm::vec2(300, 0), glm::vec2(500, 200)));
	m_textbox->SetBounds(UIRect(Quad2D(200, 250, 200, 50), glm::vec2(200, 250), glm::vec2(200, 250)));

	m_scrollBar->SetScale(11);
	m_scrollBar->SetPosition(4);

	m_hScrollBar->SetScale(11);
	m_hScrollBar->SetHorizontal(true);
	m_checkbox->SetChecked(true);

	m_button->SetText("Test Button_gy");
	m_label->SetText("This is a test");
	m_textbox->SetPlaceholder("Placeholder text");

	SetRootComponent(m_root);

	m_root->AddComponent(m_button);
	m_root->AddComponent(m_checkbox);
	m_root->AddComponent(m_scrollBar);
	m_root->AddComponent(m_hScrollBar);
	m_root->AddComponent(m_label);
	m_root->AddComponent(m_textbox);

	m_root->OnRender.Subscribe(&TestUI::RenderTest, this);
	m_checkbox->CheckedChanged.Subscribe(&TestUI::CheckedChanged, this);
	m_scrollBar->Scroll.Subscribe(&TestUI::VerticalScroll, this);
	m_hScrollBar->Scroll.Subscribe(&TestUI::HorizontalScroll, this);
	m_button->Click.Subscribe(&TestUI::ClickButton, this);
}

bool HoverColor = true;

void TestUI::RenderTest(RenderEventArgs& args)
{
}

void TestUI::CheckedChanged(BoolEventArgs& args)
{
	Entity cam = Snowfall::GetGameInstance().GetScene().GetEntityManager().FindEntitiesByName("camera1")[0];
	cam.GetComponent<CameraComponent>()->Enabled = args.NewValue;
}

void TestUI::HorizontalScroll(ScrollBarScrollEventArgs& args)
{
	Entity cam = Snowfall::GetGameInstance().GetScene().GetEntityManager().FindEntitiesByName("camera1")[0];
	cam.GetComponent<TransformComponent>()->Rotation.y = args.NewPosition * 36.f;
}

void TestUI::VerticalScroll(ScrollBarScrollEventArgs& args)
{
	Entity cam = Snowfall::GetGameInstance().GetScene().GetEntityManager().FindEntitiesByName("camera1")[0];
	cam.GetComponent<TransformComponent>()->Rotation.x = 90 - args.NewPosition * 18.f;
}

void TestUI::ClickButton(EventArgs& args)
{
	m_checkbox->SetChecked(!m_checkbox->GetChecked());
}
