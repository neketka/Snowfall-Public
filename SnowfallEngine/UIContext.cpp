#include "stdafx.h"
#include "UIContext.h"

UIContext::UIContext(int width, int height) : m_dispatchingEvents(true), m_doubleClickTime(0.5f), m_hoverTime(0.1f), 
	m_primaryButton(MouseButton::BUTTON_LEFT), m_secondaryButton(MouseButton::BUTTON_RIGHT), m_bounds(0, 0, width, height), m_held()
{
	m_inputManager = &Snowfall::GetGameInstance().GetInputManager();
}

UIContext::~UIContext()
{
	delete m_root;
}

void UIContext::UpdateContext(UIRenderer& uiRenderer, float time, bool onlyRender)
{
	bool mouseLocked = m_inputManager->IsMouseLocked();
	if (!(onlyRender || mouseLocked))
	{
		if (m_dispatchingEvents)
		{
			Quad2D mouseBox = Quad2D(RaycastPosition(m_inputManager->GetMousePosition()), glm::vec2(5, 5));
			for (MouseMoveEvent e : m_inputManager->GetEventQueue<MouseMoveEvent>()) // Find the MouseHover/Enter/Leave events + find hovering component
			{
				MouseMotionEventArgs args;
				args.OldPosition = RaycastPosition(e.OldPos);
				args.NewPosition = RaycastPosition(e.NewPos);

				mouseBox = Quad2D(args.NewPosition, glm::vec2(5, 5));
				UIComponent *comp = m_root->FindTopmostIntersector(mouseBox);
				UIComponent *primaryHeld = m_held[static_cast<int>(m_primaryButton)];

				bool compAllowed = (comp == primaryHeld) == !!primaryHeld && comp && comp->IsEnabled(); //Checks if primary is holding a component and that component is currently held in this case (non held components don't get move events)
				bool lastAllowed = (m_lastMouseHover == primaryHeld) == !!primaryHeld && m_lastMouseHover && m_lastMouseHover->IsEnabled(); //Checks if primary is holding a component and that component is last held in this case

				if (comp != m_lastMouseHover)
				{
					if (lastAllowed)
					{
						m_lastMouseHover->MouseLeave.Fire(args);
						m_lastMouseHover->MouseMove.Fire(args);
					}
					if (compAllowed)
					{
						comp->MouseEnter.Fire(args);
						comp->MouseMove.Fire(args);
					}

					m_lastMouseHover = comp;
					if (!primaryHeld)
						m_doubleClickCounter = 0;
				}
				else if (lastAllowed)
				{
					m_lastMouseHover->MouseMove.Fire(args);
				}

				for (int i = 0; i < 9; ++i)
				{
					UIComponent *c = m_held[i];
					if (c)
						c->MouseMove.Fire(args);
				}

				m_lastMouseHover = comp;
				m_lastHoverTime = Snowfall::GetGameInstance().GetTime();
			}

			if (Snowfall::GetGameInstance().GetTime() - m_lastHoverTime >= m_hoverTime) // Call MouseHover if mouse stands still for a while
			{
				MouseMotionEventArgs args;
				args.OldPosition = args.NewPosition = mouseBox.Position;
				if (m_lastMouseHover)
					m_lastMouseHover->MouseHover.Fire(args);
			}

			for (MouseButtonEvent e : m_inputManager->GetEventQueue<MouseButtonEvent>())
			{
				MouseButtonEventArgs args;
				args.MousePosition = mouseBox.Position;
				args.Button = e.Button;
				args.State = e.State;
				args.Primary = e.Button == m_primaryButton;
				args.Secondary = e.Button == m_secondaryButton;
				args.Repeating = e.Repeating;

				if (!args.State)
				{
					UIComponent **held = &m_held[static_cast<int>(e.Button)];
					if (*held && *held != m_lastMouseHover)
					{
						(*held)->MouseUp.Fire(args);
						*held = nullptr;
						m_doubleClickCounter = 0;
					}
				}

				if (m_lastMouseHover)
				{
					if (e.State)
					{
						m_lastMouseHover->MouseDown.Fire(args);
						m_held[static_cast<int>(e.Button)] = m_lastMouseHover;
						if (!e.Repeating)
							++m_doubleClickCounter;
					}
					else
					{
						m_lastMouseHover->MouseUp.Fire(args);
						m_held[static_cast<int>(e.Button)] = nullptr;

						if (m_doubleClickCounter == 1)
						{
							if (args.Primary)
							{
								m_doubleClickTiming = Snowfall::GetGameInstance().GetTime();
								++m_doubleClickCounter;
							}
							m_lastMouseHover->MouseClick.Fire(args);
						}
						else if (m_doubleClickCounter == 3 && args.Primary)
						{
							m_lastMouseHover->MouseClick.Fire(args);
							if (Snowfall::GetGameInstance().GetTime() - m_doubleClickTiming <= m_doubleClickTime)
								m_lastMouseHover->MouseDoubleClick.Fire(args);
							m_doubleClickCounter = 0;
						}
					}
				}
			}

			for (TextEvent e : m_inputManager->GetEventQueue<TextEvent>())
			{
				TextEventArgs args;
				args.Character = e.Character;

				m_focused->KeyPress.Fire(args);
			}

			for (KeyEvent e : m_inputManager->GetEventQueue<KeyEvent>())
			{
				KeyEventArgs args;
				args.Key = e.Key;
				args.ModifierFlag = e.ModifierFlag;
				args.State = e.State;
				args.Repeating = e.Repeating;

				if (args.State)
					m_focused->KeyDown.Fire(args);
				else
					m_focused->KeyUp.Fire(args);
			}

			for (MouseScrollEvent e : m_inputManager->GetEventQueue<MouseScrollEvent>())
			{
				ScrollEventArgs args;
				args.OldPosition = e.OldPos;
				args.NewPosition = e.NewPos;

				if (m_lastMouseHover)
					m_lastMouseHover->MouseScroll.Fire(args);
			}
		}
	}
	if (m_root)
		m_root->Render(&uiRenderer);
}

void UIContext::SetRootComponent(UIComponent *component)
{
	m_root = component;
	m_focused = component;
	m_lastMouseHover = nullptr;
}

void UIContext::SetBounds(IQuad2D bounds)
{
	Quad2D old = Quad2D(m_bounds);
	m_bounds = bounds;
	if (m_root)
		m_root->Resize(old, Quad2D(bounds));
}

void UIContext::SetDoubleClickTime(float time)
{
	m_doubleClickTime = time;
}

void UIContext::SetHoverTime(float time)
{
	m_hoverTime = time;
}

void UIContext::SetPrimaryButton(MouseButton button)
{
	m_primaryButton = button;
}

void UIContext::SetSecondaryButton(MouseButton button)
{
	m_secondaryButton = button;
}

void UIContext::SetInputManager(InputManager *manager)
{
	m_inputManager = manager;
}

void UIContext::SetDispatchingEvents(bool dispatching)
{
	m_dispatchingEvents = dispatching;
}

void UIContext::DispatchFocusChange(UIComponent *component)
{
	if (m_focused == component)
		return;
	if (m_root)
		m_root->UpdateFocus(m_focused, component);
	m_focused = component;
}

UIComponent *UIContext::GetFocusedComponent()
{
	return m_focused;
}

IQuad2D UIContext::GetBounds()
{
	return m_bounds;
}

float UIContext::GetDoubleClickTime()
{
	return m_doubleClickTime;
}

float UIContext::GetHoverTime()
{
	return m_hoverTime;
}

bool UIContext::IsDispatchingEvents()
{
	return m_dispatchingEvents;
}

UIComponent *UIContext::GetRootComponent()
{
	return m_root;
}
