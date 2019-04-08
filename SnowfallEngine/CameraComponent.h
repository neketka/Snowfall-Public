#pragma once
#include "ECS.h"
#include "Framebuffer.h"
#include "CommandBuffer.h"
#include "TextureAsset.h"
#include "TransformComponent.h"
#include "RenderTargetAsset.h"
#include "Quad.h"
#include "MeshAsset.h"
#include "MaterialAsset.h"
#include <glm/glm.hpp>

#include "export.h"

class CameraComponent : public Component
{
public:
	CameraComponent() : Region(0, 0, 800, 600), LayerMask(0xFFFFFFFFFFFFFFFF), ViewportIndex(0), ZNear(0.3f), ZFar(1000.0f), FovY(70.0f) {}
	bool Enabled;
	IQuad2D Region;
	LayerMask LayerMask;

	float ZNear;
	float ZFar;
	float FovY;

	int ViewportIndex;
	bool KeepInternal;

	RenderTargetAsset *RenderTarget;

	std::vector<MaterialAsset *> PostProcessStack;

	glm::mat4 ProjectionMatrix;
	glm::mat4 ViewMatrix;

	RenderTargetAsset *HdrBuffer;

	RenderTargetAsset *Downsampled[4];

	IQuad2D oldRegion;
	int colorAttachment;
};

class SNOWFALLENGINE_API CameraSystem : public ISystem
{
public:
	CameraSystem();
	~CameraSystem();
	virtual void InitializeSystem(Scene& scene) override;
	virtual void Update(float deltaTime) override;
	virtual std::string GetName() override;
	virtual std::vector<std::string> GetSystemsBefore() override;
	virtual std::vector<std::string> GetSystemsAfter() override;
	virtual bool IsMainThread() override { return false; }
private:
	void RenderSkybox(CommandBuffer& buffer, CameraComponent *camera, glm::mat4 view, TextureAsset *asset);
	MeshAsset *m_quad;
	Sampler m_sampler;
	Sampler m_shadowSamplers[3];
	Scene *m_scene;
};