#pragma once
#include "Scene.h"
#include "ECS.h"
#include "TextureAsset.h"
#include "Buffer.h"
#include <glm/glm.hpp>

#include "export.h"

enum class LightType
{
	Directional = 0, Point = 1, Spot = 2
};

class LightComponent : public Component
{
public:
	LightComponent() : LayerMask(0xFFFFFFFFFFFFFFFF) {}
	LightType Type;
	bool Enabled;
	bool Shadowing;

	LayerMask ShadowLayerMask;
	LayerMask LayerMask;

	float InnerCutoff;
	float OuterCutoff;

	glm::vec3 Color;
	float Intensity;
	float Range;
	float RangeCutoff;

	glm::mat4 lightSpace;
	TextureAsset *shadowMap;
	int highIndex;
	int middleIndex;
	int lowIndex;
};

class SNOWFALLENGINE_API LightSystem : public ISystem
{
public:
	LightSystem();
	virtual ~LightSystem() override;
	virtual void InitializeSystem(Scene& scene) override;
	virtual void Update(float deltaTime) override;
	virtual std::string GetName() override;
	virtual std::vector<std::string> GetSystemsBefore() override;
	virtual std::vector<std::string> GetSystemsAfter() override;
	virtual bool IsMainThread() override { return true; }
	inline TBuffer GetLightBuffer() { return m_lights; }
private:
	int m_currentLightHold;
	TBuffer m_lights;
	Scene *m_scene;
};