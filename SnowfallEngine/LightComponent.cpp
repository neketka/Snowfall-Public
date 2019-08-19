#include "stdafx.h"

#include "LightComponent.h"
#include "TransformComponent.h"

class LightStruct
{
public:
	glm::vec4 Position; //+ Inner Cutoff
	glm::vec4 Direction; //+ Outer Cutoff
	glm::vec4 Color; //+ Intensity

	glm::mat4 LightSpace;

	float Range;
	float RangeCutoff;

	int Type; //0:dir 1:point 2:spot

	int HighIndex;
};

std::vector<SerializationField> ComponentDescriptor<DirectionalLightComponent>::GetSerializationFields()
{
	return {
		SerializationField("Enabled", SerializationType::ByValue, offsetof(DirectionalLightComponent, Enabled), 1, InterpretValueAs::Bool),
		SerializationField("Shadowing", SerializationType::ByValue, offsetof(DirectionalLightComponent, Shadowing), 1, InterpretValueAs::Bool),
		SerializationField("ShadowLayerMask", SerializationType::ByValue, offsetof(DirectionalLightComponent, ShadowLayerMask), 8, InterpretValueAs::UInt64),
		SerializationField("LayerMask", SerializationType::ByValue, offsetof(DirectionalLightComponent, LayerMask), 8, InterpretValueAs::UInt64),
		SerializationField("Color", SerializationType::ByValue, offsetof(DirectionalLightComponent, Color), 12, InterpretValueAs::FColor3),
		SerializationField("Intensity", SerializationType::ByValue, offsetof(DirectionalLightComponent, Intensity), 4, InterpretValueAs::Float32)
	};
}

std::vector<SerializationField> ComponentDescriptor<SpotLightComponent>::GetSerializationFields()
{
	return {
		SerializationField("Enabled", SerializationType::ByValue, offsetof(SpotLightComponent, Enabled), 1, InterpretValueAs::Bool),
		SerializationField("Shadowing", SerializationType::ByValue, offsetof(SpotLightComponent, Shadowing), 1, InterpretValueAs::Bool),
		SerializationField("ShadowLayerMask", SerializationType::ByValue, offsetof(SpotLightComponent, ShadowLayerMask), 8, InterpretValueAs::UInt64),
		SerializationField("LayerMask", SerializationType::ByValue, offsetof(SpotLightComponent, LayerMask), 8, InterpretValueAs::UInt64),
		SerializationField("InnerCutoff", SerializationType::ByValue, offsetof(SpotLightComponent, InnerCutoff), 4, InterpretValueAs::Float32),
		SerializationField("OuterCutoff", SerializationType::ByValue, offsetof(SpotLightComponent, OuterCutoff), 4, InterpretValueAs::Float32),
		SerializationField("Color", SerializationType::ByValue, offsetof(SpotLightComponent, Color), 12, InterpretValueAs::FColor3),
		SerializationField("Intensity", SerializationType::ByValue, offsetof(SpotLightComponent, Intensity), 4, InterpretValueAs::Float32),
		SerializationField("Range", SerializationType::ByValue, offsetof(SpotLightComponent, Range), 4, InterpretValueAs::Float32),
		SerializationField("RangeCutoff", SerializationType::ByValue, offsetof(SpotLightComponent, RangeCutoff), 4, InterpretValueAs::Float32)
	};
}

LightSystem::LightSystem()
{
	int data[33];
	data[0] = 0;
	m_currentLightHold = 0;
	m_lights = TBuffer(data, 1, 4 * 33 + CASCADING_SIZE, BufferOptions(false, false, false, false, true, false));
}

LightSystem::~LightSystem()
{
	m_lights.Destroy(); 
}

void LightSystem::InitializeSystem(Scene& scene)
{
	m_scene = &scene;
}

void LightSystem::Update(float deltaTime)
{
	std::vector<LightStruct> lights;
	glm::vec3 origin = m_scene->GetSystemManager().GetSystem<TransformSystem>()->GetRebaseOrigin();
	for (DirectionalLightComponent *comp : m_scene->GetComponentManager().GetComponents<DirectionalLightComponent>())
	{
		if (!comp->Enabled)
			continue;
		TransformComponent *tcomp = comp->Owner.GetComponent<TransformComponent>();
		LightStruct sct;
		sct.Type = 0;
		sct.Position = glm::vec4(tcomp->GlobalPosition - origin, 0);
		sct.Direction = glm::vec4(tcomp->GlobalDirection, 0);
		sct.Color = glm::vec4(comp->Color, comp->Intensity);
		sct.HighIndex = comp->Shadowing ? 0 : -1;

		lights.push_back(sct);
	}
	for (SpotLightComponent *comp : m_scene->GetComponentManager().GetComponents<SpotLightComponent>())
	{
		if (!comp->Enabled)
			continue;
		TransformComponent *tcomp = comp->Owner.GetComponent<TransformComponent>();

		LightStruct sct;
		sct.Type = 2;
		sct.Position = glm::vec4(tcomp->GlobalPosition - origin, std::cosf(glm::radians(comp->InnerCutoff)));
		sct.Direction = glm::vec4(tcomp->GlobalDirection, std::cosf(glm::radians(comp->OuterCutoff)));
		sct.Range = comp->Range;
		sct.RangeCutoff = comp->RangeCutoff;
		sct.HighIndex = comp->index;
		sct.Color = glm::vec4(comp->Color, comp->Intensity);
		sct.LightSpace = comp->lightSpaceMatrix;

		lights.push_back(sct);
	}
	if (m_currentLightHold < lights.size())
	{
		m_lights.Destroy();
		m_lights = TBuffer(nullptr, 1, CASCADING_SIZE + sizeof(int) * 32 + sizeof(LightStruct) * (lights.size() + 2),
			BufferOptions(false, false, false, false, true, false));
		m_currentLightHold = lights.size() + 2;
	}

	DirectionalLightComponent *dir = m_scene->GetComponentManager().GetComponents<DirectionalLightComponent>()[0];

	m_lights.CopyData(dir->lightSpaceDistances, 0, LIGHT_CASCADES * sizeof(float));
	m_lights.CopyData(dir->indices, LIGHT_CASCADES * sizeof(float), LIGHT_CASCADES * sizeof(int));
	m_lights.CopyData(dir->lightSpaceMatrices, LIGHT_CASCADES * (sizeof(float) + sizeof(int)) + (LIGHT_CASCADES % 2) * sizeof(int), LIGHT_CASCADES * sizeof(glm::mat4));
	
	m_lights.CopyData(lights.data(), sizeof(int) * 32 + CASCADING_SIZE, sizeof(LightStruct) * lights.size());
}

std::string LightSystem::GetName()
{
	return "LightSystem";
}

std::vector<std::string> LightSystem::GetSystemsBefore()
{
	return { "TransformSystem" };
}

std::vector<std::string> LightSystem::GetSystemsAfter()
{
	return std::vector<std::string>();
}
