#pragma once
#include "ECS.h"
#include "RenderTargetAsset.h"

class EyeAdaptationComponent : public Component
{
public:
	float MinLuma;
	float MaxLuma;
	float KeyValue;
	float ExposureBias;
	float EaseConstant;

	MaterialAsset *TonemappingMaterial;
	MaterialAsset *BloomMaterial;
	
	IQuad2D oldRegion;
	RenderTargetAsset *renderTarget;
	RenderTargetAsset *renderTargetEase;
	TextureAsset *mipView;
	Sampler sampler;
	Sampler sampler2;
};

class EyeAdaptationSystem : public ISystem
{
public:
	virtual void InitializeSystem(Scene& scene) override;
	virtual void Update(float deltaTime) override;
	virtual std::string GetName() override;
	virtual std::vector<std::string> GetSystemsBefore() override;
	virtual std::vector<std::string> GetSystemsAfter() override;
	virtual bool IsMainThread() override;
private:
	void RunEasingFunction(EyeAdaptationComponent *comp, float deltaT, int level);
	Scene *m_scene;
};

