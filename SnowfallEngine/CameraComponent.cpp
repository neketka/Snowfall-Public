#include "CameraComponent.h"
#include "TransformComponent.h"
#include "CommandBuffer.h"
#include "Snowfall.h"
#include "Scene.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

std::vector<SerializationField> ComponentDescriptor<CameraComponent>::GetSerializationFields()
{
	return {
		SerializationField("Region", SerializationType::ByValue, sizeof(IQuad2D), InterpretValueAs::IVector2Array),
		SerializationField("LayerMask", SerializationType::ByValue, sizeof(LayerMask), InterpretValueAs::UInt64),
		SerializationField("ZNear", SerializationType::ByValue, sizeof(float), InterpretValueAs::Float32),
		SerializationField("ZFar", SerializationType::ByValue, sizeof(float), InterpretValueAs::Float32),
		SerializationField("FovY", SerializationType::ByValue, sizeof(float), InterpretValueAs::Float32)
	};
}

void CameraSystem::InitializeSystem(Scene& scene)
{
	m_scene = &scene;
}

void CameraSystem::Update(float deltaTime)
{
	for (CameraComponent *camera : m_scene->GetComponentManager().GetComponents<CameraComponent>())
	{
		camera->RenderTarget.ClearColor(0, glm::vec4(0, 0, 0, 1));
		camera->RenderTarget.ClearDepth(1);

		TransformComponent *transform = camera->Owner.GetComponent<TransformComponent>();
		camera->ProjectionMatrix = glm::perspective(camera->FovY, static_cast<float>(camera->Region.Size.x) / static_cast<float>(camera->Region.Size.y), camera->ZNear, camera->ZFar);
		camera->ViewMatrix = glm::rotate(transform->Rotation.y, glm::vec3(0, 1, 0)) *
			glm::rotate(transform->Rotation.x, glm::vec3(1, 0, 0)) *
			glm::rotate(transform->Rotation.z, glm::vec3(0, 0, 1)) *
			glm::translate(transform->Position);
	}

	Snowfall::GetGameInstance().GetMeshManager().RunCullingPass({});

	CommandBuffer buffer;
	for (CameraComponent *camera : m_scene->GetComponentManager().GetComponents<CameraComponent>())
	{
		Pipeline pipeline;
		pipeline.FragmentStage.DepthTest = true;
		pipeline.FragmentStage.Framebuffer = camera->RenderTarget;
		pipeline.FragmentStage.Viewport = camera->Region;
		pipeline.FragmentStage.DrawTargets = { 0 };

		ShaderConstants constants;
		constants.AddConstant(0, camera->ProjectionMatrix);
		constants.AddConstant(1, camera->ViewMatrix);

		Snowfall::GetGameInstance().GetMeshManager().Render(buffer, pipeline, constants, ShaderDescriptor(), camera->LayerMask, false);
	}
	buffer.ExecuteCommands();
}

std::string CameraSystem::GetName()
{
	return "CameraSystem";
}

std::vector<std::string> CameraSystem::GetSystemsBefore()
{
	return { "TransformSystem" };
}

std::vector<std::string> CameraSystem::GetSystemsAfter()
{
	return {};
}
