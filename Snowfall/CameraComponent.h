#pragma once
#include "ECS.h"
#include "Framebuffer.h"
#include "Quad.h"
#include <glm/glm.hpp>

class CameraComponent : public Component
{
public:
	CameraComponent() : Region(0, 0, 800, 600), LayerMask(0xFFFFFFFFFFFFFFFF), ZNear(0.3f), ZFar(1000.0f), FovY(1.57079633f), RenderTarget(Framebuffer::GetDefault()) {}
	IQuad2D Region;
	LayerMask LayerMask;

	float ZNear;
	float ZFar;
	float FovY;

	Framebuffer RenderTarget;
	glm::mat4 ProjectionMatrix;
	glm::mat4 ViewMatrix;
};

class CameraSystem : public ISystem
{
public:
	virtual void InitializeSystem(EntityManager *eManager, ComponentManager *cManager, EventManager *evManager, PrototypeManager *pManager) override;
	virtual void Update(float deltaTime) override;
	virtual std::string GetName() override;
	virtual std::vector<std::string> GetSystemsBefore() override;
	virtual std::vector<std::string> GetSystemsAfter() override;
	virtual bool IsMainThread() override { return false; }
private:
	EntityManager *m_eManager;
	ComponentManager *m_cManager;
};