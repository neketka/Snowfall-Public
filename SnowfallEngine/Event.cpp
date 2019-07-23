#include "stdafx.h"
#include "ECS.h"

std::vector<IEvent *> EventManager::ListenEvents(std::string system)
{
	std::vector<IEvent *> events = std::vector<IEvent *>(m_eventsPerSubscriber[system]);
	m_eventsPerSubscriber[system].clear();
	return events;
}

void EventManager::SubscribeEvent(std::string system, std::string name)
{
	if (m_subscribers.find(name) == m_subscribers.end())
		m_subscribers.insert({ name, std::vector<std::string>() });
	if (m_eventsPerSubscriber.find(system) == m_eventsPerSubscriber.end())
		m_eventsPerSubscriber.insert({ name, std::vector<IEvent *>() });
	m_subscribers[name].push_back(system);
}

void EventManager::PushEvent(std::string name, IEvent *event)
{
	event->Name = name;
	auto iter = m_subscribers.end();
	if ((iter = m_subscribers.find(name)) == m_subscribers.end())
		return;
	for (std::string system : iter->second)
		m_eventsPerSubscriber[system].push_back(event);
}

void EventManager::SubscribeEntityEvent(std::string system, std::string name, Entity e)
{
	SubscribeEvent(system, name + e.GetId());
}

void EventManager::PushEntityEvent(std::string name, Entity e, IEvent *event)
{
	event->Name = name;
	event->Entity = e;

	auto iter = m_subscribers.end();
	if ((iter = m_subscribers.find(name + e.GetId())) == m_subscribers.end())
		return;
	for (std::string system : iter->second)
		m_eventsPerSubscriber[system].push_back(event);
}
