#pragma once
#include <Module.h>

class SnowfallGame : public Module
{
public:
	virtual void InitializeModule() override;
	virtual void InitializePrototypes(PrototypeManager& manager) override;
	virtual void ReleaseModule() override;
	virtual std::string GetModuleName() override;
};

extern "C" __declspec(dllexport) Module *CreateModule();