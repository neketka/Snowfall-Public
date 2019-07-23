#include <Snowfall.h>
#include <UIEvent.h>
#include <PhysicsRigidBodySystem.h>
#include <TransformComponent.h>

#include "TestUI.h"
#include <MeshComponent.h>

TestUI::TestUI() : UIContext(800, 600)
{
	m_root = new UIComponent;
	m_send = new UIButton;

	m_root->SetBounds(UIRect(Quad2D(0, 0, 800, 600), glm::vec2(0, 0), glm::vec2(800, 600)));
	m_send->SetBounds(UIRect(Quad2D(100, 100, 100, 40), glm::vec2(100, 100), glm::vec2(200, 140)));
	m_send->SetText("Send");

	m_root->SetContext(this);

	SetRootComponent(m_root);

	m_root->AddComponent(m_send);
	
	m_root->OnRender.Subscribe(&TestUI::RenderTest, this);
	m_send->Click.Subscribe(&TestUI::Send, this);
}

bool HoverColor = true;

void TestUI::RenderTest(RenderEventArgs& args)
{
}

void TestUI::Send(EventArgs& args)
{
	Entity box = Snowfall::GetGameInstance().GetScene().GetEntityManager().FindEntitiesByName("Box")[0];
	Entity camera = Snowfall::GetGameInstance().GetScene().GetEntityManager().FindEntitiesByName("MainCam")[0];

	Entity cloned = box.Clone();
	auto cComp = camera.GetComponent<TransformComponent>();
	auto pos = cloned.GetComponent<TransformComponent>()->Position = camera.GetComponent<TransformComponent>()->GlobalPosition + camera.GetComponent<TransformComponent>()->GlobalDirection;
	cloned.GetComponent<PhysicsRigidBodyComponent>()->LinearVelocity = camera.GetComponent<TransformComponent>()->GlobalDirection * 5.f;
}
