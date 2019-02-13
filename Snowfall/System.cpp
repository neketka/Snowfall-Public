#include "ECS.h"
#include <algorithm>

SystemManager::~SystemManager()
{
	for (auto sysOrdered : m_systemGroups)
	{
		for (ISystem *sys : sysOrdered)
		{
			delete sys;
		}
	}
}

void SystemManager::AddSystem(ISystem *system)
{
	m_systems.insert({ system->GetName(), system });
	system->Group = m_systemGroups.size();
	m_systemGroups.push_back(std::vector<ISystem *> { system });
}

ISystem *SystemManager::GetSystem(std::string name)
{
	return m_systems.at(name);
}

void SystemManager::InitializeSystems()
{
	for (auto sysPair : m_systems)
	{
		sysPair.second->InitializeSystem(m_eManager, m_cManager, m_evManager, m_pManager);
	}
	for (int i = 0; i < m_systemGroups.size(); ++i)
	{
		bool addedToThread = false;
		std::vector<ISystem *>& tGroup = m_systemGroups[i];
		if (tGroup.size() == 0)
		{
			m_systemGroups.erase(m_systemGroups.begin() + i);
			--i;
		}
		else
		{
			for (ISystem *sys : tGroup)
			{
				sys->Group = i;
				if (sys->IsMainThread())
				{
					m_mainThreadGroups.push_back(i);
					addedToThread = true;
				}
			}
			if (!addedToThread)
				m_anyThreadGroups.push_back(i);
		}
	}
}

void SystemManager::UpdateSystems(float deltaTime, int group)
{
	if (group == -1)
	{
		for (auto sysOrdered : m_systemGroups)
		{
			for (ISystem *sys : sysOrdered)
			{
				if (sys->IsEnabled())
					sys->Update(deltaTime);
			}
		}
	}
	else
	{
		for (ISystem *sys : m_systemGroups[group])
		{
			if (sys->IsEnabled())
				sys->Update(deltaTime);
		}
	}
}
