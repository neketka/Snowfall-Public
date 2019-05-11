#pragma once

#include "ECS.h"
#include <string>

class Module
{
public:
	virtual void InitializeModule() = 0;
	virtual void InitializePrototypes(PrototypeManager& manager) = 0;
	virtual void ReleaseModule() = 0;
	virtual std::string GetModuleName() = 0;
};

