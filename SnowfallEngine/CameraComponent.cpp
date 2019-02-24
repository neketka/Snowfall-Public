#include "CameraComponent.h"
#include "TransformComponent.h"
#include "CommandBuffer.h"
#include "Snowfall.h"
#include "LightComponent.h"
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
	Snowfall::GetGameInstance().GetMeshManager().RunCullingPass({});

	TBuffer lights = dynamic_cast<LightSystem *>(m_scene->GetSystemManager().GetSystem("LightSystem"))->GetLightBuffer();
	CommandBuffer buffer;
	for (CameraComponent *camera : m_scene->GetComponentManager().GetComponents<CameraComponent>())
	{
		TransformComponent *transform = camera->Owner.GetComponent<TransformComponent>();

		if (camera->LockToTargetResolution)
		{
			if (camera->RenderTarget.GetID() == 0)
				camera->Region = IQuad2D(glm::ivec2(0, 0), Snowfall::GetGameInstance().GetViewportSize());
		}

		camera->RenderTarget.ClearColor(0, glm::vec4(0, 0, 0, 1));
		camera->RenderTarget.ClearDepth(1);

		camera->ProjectionMatrix = glm::perspective(glm::radians(camera->FovY), static_cast<float>(camera->Region.Size.x) / static_cast<float>(camera->Region.Size.y), camera->ZNear, camera->ZFar);
		camera->ViewMatrix = glm::mat4(glm::mat3(transform->ModelMatrix)) * glm::translate(-transform->GlobalPosition);

		std::vector<int> indices;
		int index = 0;
		for (LightComponent *light : m_scene->GetComponentManager().GetComponents<LightComponent>())
		{
			if (index == 31)
				break;
			indices.push_back(index);
			++index;
		}

		lights.CopyData(&index, 0, 4);
		lights.CopyData(indices.data(), 4, 4 * index);

		Pipeline pipeline;
		pipeline.FragmentStage.DepthTest = true;
		pipeline.FragmentStage.DepthMask = true;
		pipeline.FragmentStage.Framebuffer = camera->RenderTarget;
		pipeline.FragmentStage.Viewport = camera->Region;
		pipeline.FragmentStage.DrawTargets = { 0 };
		pipeline.VertexStage.BackFaceCulling = true;

		ShaderConstants constants;
		constants.AddConstant(0, camera->ProjectionMatrix);
		constants.AddConstant(1, camera->ViewMatrix);
		constants.AddConstant(4, transform->Position);

		ShaderDescriptor desc;
		desc.AddShaderStorageBuffer(lights, 2);

		Snowfall::GetGameInstance().GetMeshManager().Render(buffer, pipeline, constants, desc, camera->LayerMask, {}, false);
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
