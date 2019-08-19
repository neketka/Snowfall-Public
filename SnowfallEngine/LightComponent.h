#pragma once
#include "Scene.h"
#include "ECS.h"
#include "TextureAsset.h"
#include "Buffer.h"
#include <glm/glm.hpp>

#include "export.h"

#define LIGHT_CASCADES 4

#define CASCADING_SIZE (sizeof(float) * LIGHT_CASCADES + sizeof(int) * LIGHT_CASCADES + sizeof(int) * (LIGHT_CASCADES % 2) \
+ sizeof(glm::mat4) * LIGHT_CASCADES)

enum class LightType
{
	Directional = 0, Point = 1, Spot = 2
};

class DirectionalLightComponent : public Component
{
public:
	DirectionalLightComponent() : LayerMask(0xFFFFFFFFFFFFFFFF) {}
	bool Enabled;
	bool Shadowing;

	LayerMask ShadowLayerMask;
	LayerMask LayerMask;

	glm::vec3 Color;
	float Intensity;

	float CascadeSizes[LIGHT_CASCADES];
	Entity CascadeCamera;

	glm::mat4 lightSpaceMatrices[LIGHT_CASCADES];
	float lightSpaceDistances[LIGHT_CASCADES];
	int indices[LIGHT_CASCADES];
	TextureAsset *shadowMap;
};

class SpotLightComponent : public Component
{
public:
	SpotLightComponent() : LayerMask(0xFFFFFFFFFFFFFFFF) {}
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

	int index;
	glm::mat4 lightSpaceMatrix;
	TextureAsset *shadowMap;
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