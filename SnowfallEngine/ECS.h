#pragma once
#include <map>
#include <vector>
#include <string>
#include <istream>
#include <functional>
#include <queue>
#include "AssetManager.h"

#include "export.h"

//using EntityId = long;
using EntityId = std::string;
using LayerMask = unsigned long long;

class Scene;

enum class BooleanStateUpdate
{
	Unchanged, True, False
};

enum class SerializationType
{
	NonSerializable, Entity, String, Asset, EntityVector, StringVector, AssetVector, ByValue, ByValueVector
};

enum class InterpretValueAs
{
	NotApplicable, Entity, String, Bool, Int8, Int8Array, Int16, Int16Array, Int32, Int32Array, Int64, Int64Array,
	UInt8, UInt8Array, UInt16, UInt16Array, UInt32, UInt32Array, UInt64, UInt64Array,
	Float32, Float32Array, Float64, Float64Array, FVector2, FVector2Array, FVector3, 
	FVector3Array, FVector4, FVector4Array, IVector2, IVector2Array, IVector3,
	IVector3Array, IVector4, IVector4Array, FColor3, FColor4, FMatrix3, FMatrix4, 
	AnyAsset, MaterialAsset, MeshAsset, ShaderAsset, TextureAsset, RenderTargetAsset, FontAsset
};

class SerializationField
{
public:
	SerializationField(std::string name, SerializationType type, int offset, int size, InterpretValueAs valueType) 
		: Name(name), Type(type), Offset(offset), Size(size), ValueType(valueType) { }
	SerializationType Type;
	InterpretValueAs ValueType;
	int Offset;
	int Size;
	std::string Name;
};

class Component;

template<class T>
class ComponentDescriptor
{
public:
	static std::string GetName() { return std::string(typeid(T).name() + 6); }
	static int GetSize() { return sizeof(T); }
	static Component *InitializeComponent(char *memory) { return new (memory)T; }
	static Component *CopyComponent(Component *c, char *memory) { return new (memory)T(*static_cast<T*>(c)); }
	static std::vector<SerializationField> GetSerializationFields();
};

typedef Component *(*InitializeComponentFunc)(char *);
typedef Component *(*CopyComponentFunc)(Component *, char *);

class ComponentDescription
{
public:
	ComponentDescription() { }
	ComponentDescription(std::string name, int size, std::vector<SerializationField> serializationFields, InitializeComponentFunc initComponent, CopyComponentFunc copyComponent) : Name(name),
		Size(size), SerializationFields(serializationFields), InitializeComponent(initComponent), CopyComponent(CopyComponent) { }
	std::string Name;
	int Size = 0; 
	InitializeComponentFunc InitializeComponent;
	CopyComponentFunc CopyComponent;
	std::vector<SerializationField> SerializationFields;
};

class ISystem;

class ISystemFactory 
{
public:
	virtual ISystem *CreateInstance() = 0;
};

template<class T>
class SystemFactory : public ISystemFactory
{
public:
	virtual ISystem *CreateInstance() override { return dynamic_cast<ISystem *>(new T); }
	static std::string GetName() { return std::string(typeid(T).name() + 6); }
};

class PrototypeManager 
{
public:
	SNOWFALLENGINE_API PrototypeManager() { }
	SNOWFALLENGINE_API ~PrototypeManager() 
	{ 
		for (auto kp : m_systemPrototypes)
			delete kp.second;
	}
	SNOWFALLENGINE_API std::vector<SerializationField>& GetSerializationComponentFields(std::string name);
	SNOWFALLENGINE_API int GetComponentSize(std::string name);
	SNOWFALLENGINE_API InitializeComponentFunc GetInitializer(std::string name);
	SNOWFALLENGINE_API CopyComponentFunc GetCopy(std::string name);
	template<class T>
	void AddComponentDescription()
	{
		ComponentDescription desc;
		desc.Name = ComponentDescriptor<T>::GetName();
		desc.Size = ComponentDescriptor<T>::GetSize();
		desc.InitializeComponent = ComponentDescriptor<T>::InitializeComponent;
		desc.CopyComponent = ComponentDescriptor<T>::CopyComponent;
		desc.SerializationFields = ComponentDescriptor<T>::GetSerializationFields();
		m_componentDescriptions[desc.Name] = desc;
	}

	ISystem *CreateSystem(std::string name)
	{
		return m_systemPrototypes[name]->CreateInstance();
	}

	template<class T>
	void AddSystemPrototype()
	{
		std::string name = std::string(typeid(T).name() + 6);
		m_systemPrototypes.insert({ name, new SystemFactory<T> });
	}

	void AddComponentDescription(ComponentDescription desc)
	{
		m_componentDescriptions[desc.Name] = desc;
	}
private:
	std::map<std::string, ComponentDescription> m_componentDescriptions;
	std::map<std::string, ISystemFactory *> m_systemPrototypes;
};

class Entity;

class ComponentManager
{
public:
	SNOWFALLENGINE_API ComponentManager(Scene& scene) : m_scene(scene) {}
	SNOWFALLENGINE_API ~ComponentManager();
	SNOWFALLENGINE_API Component *CreateComponent(std::string name);
	SNOWFALLENGINE_API Component *CreateComponentRaw(std::string name);
	SNOWFALLENGINE_API std::vector<Component *> GetComponents(std::string name);
	SNOWFALLENGINE_API std::vector<Component *> GetDeadComponents(std::string name);
	template<class T>
	std::vector<T *> GetComponents()
	{
		return *reinterpret_cast<std::vector<T *>*>(&GetComponents(ComponentDescriptor<T>::GetName()));
	}
	template<class T>
	std::vector<T *> GetDeadComponents()
	{
		return *reinterpret_cast<std::vector<T *>*>(&GetDeadComponents(ComponentDescriptor<T>::GetName()));
	}
	SNOWFALLENGINE_API void CleanDeadComponents();
	SNOWFALLENGINE_API void DeleteComponent(Component *component);
	SNOWFALLENGINE_API void DeleteComponentRaw(Component *component);
	SNOWFALLENGINE_API void SerializeComponent(Component *component, IAssetStreamIO& stream);
	SNOWFALLENGINE_API Component *CloneComponent(Component *component);
	SNOWFALLENGINE_API Component *DeserializeComponent(IAssetStreamIO& stream);
	SNOWFALLENGINE_API Component *DeserializeComponentRaw(IAssetStreamIO& stream);
private:
	long m_deadAddedLast = 0;
	long m_deadAddedNext = 0;
	std::map<std::string, std::vector<Component *>> m_components;
	std::map<std::string, std::vector<Component *>> m_deadComponents;
	std::queue<Component *> m_toKill;
	Scene& m_scene;
};

class EntityManager
{
public:
	SNOWFALLENGINE_API EntityManager(Scene& scene) : m_scene(scene) { }
	SNOWFALLENGINE_API Entity CreateEntity(std::vector<std::string> components);
	SNOWFALLENGINE_API Component *GetComponent(EntityId id, std::string component);
	SNOWFALLENGINE_API std::vector<Component *>& GetComponents(EntityId id);
	SNOWFALLENGINE_API void KillEntity(EntityId id);
	SNOWFALLENGINE_API Entity CloneEntity(EntityId id);

	SNOWFALLENGINE_API void AddComponent(EntityId id, std::string component);
	SNOWFALLENGINE_API void RemoveComponent(EntityId id, std::string component);

	SNOWFALLENGINE_API std::vector<Entity> FindEntitiesByName(std::string name);

	SNOWFALLENGINE_API std::string GetName(EntityId id);
	SNOWFALLENGINE_API void SetName(EntityId id, std::string name);

	SNOWFALLENGINE_API void SerializeEntity(EntityId id, IAssetStreamIO& stream);
	SNOWFALLENGINE_API std::vector<Entity> LoadEntities(int count, IAssetStreamIO& stream);

	SNOWFALLENGINE_API int GetEntityCount();
private:
	Scene& m_scene;
	std::map<EntityId, std::vector<Component *>> m_entities;
	std::map<EntityId, std::string> m_entNamePair;
};

class Entity
{
public:
	Entity(EntityId id, EntityManager *manager) : m_id(id), m_manager(manager) { }
	Entity() : m_id(""), m_manager(nullptr) { }

	inline EntityId GetId() { return m_id; }

	template<class T>
	void AddComponent() { return static_cast<T *>(m_manager->AddComponent(m_id, ComponentDescriptor<T>::GetName())); }
	template<class T>
	void RemoveComponent() { return static_cast<T *>(m_manager->RemoveComponent(m_id, ComponentDescriptor<T>::GetName())); }

	Entity Clone() { return m_manager->CloneEntity(m_id); }
	void Kill() { m_manager->KillEntity(m_id); }

	std::string GetName() { return m_manager->GetName(m_id); }
	void SetName(std::string name) { m_manager->SetName(m_id, name); }

	template<class T>
	T *GetComponent() { return static_cast<T *>(m_manager->GetComponent(m_id, ComponentDescriptor<T>::GetName())); }

	bool operator==(Entity& other) const { return m_id == other.m_id; }
	bool operator!=(Entity& other) const { return m_id != other.m_id; }

	bool IsNull() { return m_id == ""; }
private:
	EntityId m_id;
	EntityManager *m_manager;
};

class IEvent
{
public:
	std::string Name;
	Entity Entity;
};

class Component
{
public:
	Entity Owner;
	std::string InternalName;
	bool Copied = false;
};

class EventManager
{
public:
	EventManager() { }
	SNOWFALLENGINE_API std::vector<IEvent *> ListenEvents(std::string system);

	SNOWFALLENGINE_API void SubscribeEvent(std::string system, std::string name);
	SNOWFALLENGINE_API void PushEvent(std::string name, IEvent *event);

	SNOWFALLENGINE_API void SubscribeEntityEvent(std::string system, std::string name, Entity e);
	SNOWFALLENGINE_API void PushEntityEvent(std::string name, Entity e, IEvent *event);
private:
	std::map<std::string, std::vector<std::string>> m_subscribers;
	std::map<std::string, std::vector<IEvent *>> m_eventsPerSubscriber;
};

class SNOWFALLENGINE_API ISystem
{
public:
	virtual ~ISystem() {}
	virtual void InitializeSystem(Scene& scene) = 0;
	virtual void Update(float deltaTime) = 0;
	virtual std::string GetName() = 0;
	virtual std::vector<std::string> GetSystemsBefore() = 0;
	virtual std::vector<std::string> GetSystemsAfter() = 0;
	virtual bool IsMainThread() = 0;

	inline void SetEnabled(bool enabled) { m_enabled = enabled; }
	inline bool IsEnabled() { return m_enabled; }

	int Group = -1;
private:
	bool m_enabled = false;
};

class SystemManager
{
public:
	SNOWFALLENGINE_API SystemManager(Scene& scene) : m_scene(scene) { }
	SNOWFALLENGINE_API ~SystemManager();
	SNOWFALLENGINE_API void AddSystem(ISystem *system);
	SNOWFALLENGINE_API void AddSystem(std::string name);
	SNOWFALLENGINE_API void AddEnabledSystems(std::vector<std::string> names);
	SNOWFALLENGINE_API ISystem *GetSystem(std::string name);
	template<class T>
	T *GetSystem()
	{
		return dynamic_cast<T *>(GetSystem(SystemFactory<T>::GetName()));
	}
	SNOWFALLENGINE_API void InitializeSystems();
	SNOWFALLENGINE_API void UpdateSystems(float deltaTime, int group=-1);
	SNOWFALLENGINE_API inline std::vector<int> GetMainThreadGroups() { return m_mainThreadGroups; }
	SNOWFALLENGINE_API inline std::vector<int> GetAnyThreadGroups() { return m_anyThreadGroups; }
private:
	std::vector<int> m_mainThreadGroups;
	std::vector<int> m_anyThreadGroups;

	std::map<std::string, ISystem *> m_systems;
	std::vector<std::vector<ISystem *>> m_systemGroups;
	Scene& m_scene;
};