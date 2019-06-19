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

	float Padding0[2];

	int Type; //0:dir 1:point 2:spot

	int HighIndex;
	int MiddleIndex;
	int LowIndex;
};

std::vector<SerializationField> ComponentDescriptor<LightComponent>::GetSerializationFields()
{
	return {
		SerializationField("Enabled", SerializationType::ByValue, offsetof(LightComponent, Enabled), 1, InterpretValueAs::Bool),
		SerializationField("Shadowing", SerializationType::ByValue, offsetof(LightComponent, Shadowing), 1, InterpretValueAs::Bool),
		SerializationField("ShadowLayerMask", SerializationType::ByValue, offsetof(LightComponent, ShadowLayerMask), 8, InterpretValueAs::UInt64),
		SerializationField("LayerMask", SerializationType::ByValue, offsetof(LightComponent, LayerMask), 8, InterpretValueAs::UInt64),
		SerializationField("InnerCutoff", SerializationType::ByValue, offsetof(LightComponent, InnerCutoff), 4, InterpretValueAs::Float32),
		SerializationField("OuterCutoff", SerializationType::ByValue, offsetof(LightComponent, OuterCutoff), 4, InterpretValueAs::Float32),
		SerializationField("Color", SerializationType::ByValue, offsetof(LightComponent, Color), 12, InterpretValueAs::FColor3),
		SerializationField("Intensity", SerializationType::ByValue, offsetof(LightComponent, Intensity), 4, InterpretValueAs::Float32),
		SerializationField("Range", SerializationType::ByValue, offsetof(LightComponent, Range), 4, InterpretValueAs::Float32),
		SerializationField("RangeCutoff", SerializationType::ByValue, offsetof(LightComponent, RangeCutoff), 4, InterpretValueAs::Float32)
	};
}

LightSystem::LightSystem()
{
	int data[33];
	data[0] = 0;
	m_currentLightHold = 0;
	m_lights = TBuffer(data, 1, 4 * 33, BufferOptions(false, false, false, false, true, false));
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
	for (LightComponent *comp : m_scene->GetComponentManager().GetComponents<LightComponent>())
	{
		if (!comp->Enabled)
			continue;
		TransformComponent *tcomp = comp->Owner.GetComponent<TransformComponent>();
		LightStruct sct;
		sct.Type = static_cast<int>(comp->Type);
		sct.Position = glm::vec4(tcomp->GlobalPosition, std::cosf(glm::radians(comp->InnerCutoff)));
		sct.Direction = glm::vec4(tcomp->GlobalDirection, std::cosf(glm::radians(comp->OuterCutoff)));
		sct.Range = comp->Range;
		sct.RangeCutoff = comp->RangeCutoff;
		sct.HighIndex = comp->highIndex;
		sct.MiddleIndex = comp->middleIndex;
		sct.LowIndex = comp->lowIndex;
		sct.Color = glm::vec4(comp->Color, comp->Intensity);
		sct.LightSpace = comp->lightSpace;
		lights.push_back(sct);
	}
	if (m_currentLightHold < lights.size())
	{
		m_lights.Destroy();
		m_lights = TBuffer(nullptr, 1, sizeof(int) * 32 + sizeof(LightStruct) * (lights.size() + 2), 
			BufferOptions(false, false, false, false, true, false));
		m_currentLightHold = lights.size() + 2;
	}
	m_lights.CopyData(lights.data(), sizeof(int) * 32, sizeof(LightStruct) * lights.size()); 
	
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
