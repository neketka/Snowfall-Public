#pragma once
#include <map>
#include <vector>
#include <string>
#include <istream>
#include <functional>
#include <queue>
#include "AssetManager.h"

//using EntityId = long;
using EntityId = std::string;
using LayerMask = unsigned long long;

enum class SerializationType
{
	NonSerializable, Entity, String, Asset, EntityVector, StringVector, AssetVector, ByValue, ByValueVector
};

enum class InterpretValueAs
{
	NotApplicable, Int8, Int8Array, Int16, Int16Array, Int32, Int32Array, Int64, Int64Array,
	UInt8, UInt8Array, UInt16, UInt16Array, UInt32, UInt32Array, UInt64, UInt64Array,
	Float32, Float32Array, Float64, Float64Array, FVector2, FVector2Array, FVector3, 
	FVector3Array, FVector4, FVector4Array, IVector2, IVector2Array, IVector3,
	IVector3Array, IVector4, IVector4Array, AnyAsset, MaterialAsset, MeshAsset, ShaderAsset, TextureAsset
};

class SerializationField
{
public:
	SerializationField(std::string name, SerializationType type, int size, InterpretValueAs valueType) 
		: Name(name), Type(type), Size(size) { }
	SerializationType Type;
	InterpretValueAs ValueType;
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

class ComponentDescription
{
public:
	ComponentDescription() { }
	ComponentDescription(std::string name, int size, std::vector<SerializationField> serializationFields, std::function<Component *(char *)> initComponent) : Name(name),
		Size(size), SerializationFields(serializationFields), InitializeComponent(initComponent) { }
	std::string Name;
	int Size; 
	std::function<Component *(char *)> InitializeComponent;
	std::function<Component *(Component *, char *)> CopyComponent;
	std::vector<SerializationField> SerializationFields;
};

class PrototypeManager 
{
public:
	PrototypeManager() { }
	std::vector<SerializationField>& GetSerializationComponentFields(std::string name);
	int GetComponentSize(std::string name);
	std::function<Component *(char *)> GetInitializer(std::string name);
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
	void AddComponentDescription(ComponentDescription desc)
	{
		m_componentDescriptions[desc.Name] = desc;
	}
private:
	std::map<std::string, ComponentDescription> m_componentDescriptions;
};

class Entity;

class IEvent
{
public:
	std::string Name;
};

class ComponentManager
{
public:
	ComponentManager(PrototypeManager *pManager) : m_pManager(pManager) {}
	~ComponentManager();
	Component *CreateComponent(std::string name);
	std::vector<Component *>& GetComponents(std::string name);
	std::vector<Component *>& GetDeadComponents(std::string name);
	template<class T>
	std::vector<T *>& GetComponents()
	{
		return reinterpret_cast<std::vector<T *>&>(GetComponents(ComponentDescriptor<T>::GetName()));
	}
	template<class T>
	std::vector<T *>& GetDeadComponents()
	{
		return reinterpret_cast<std::vector<T *>&>(GetDeadComponents(ComponentDescriptor<T>::GetName()));
	}
	void CleanDeadComponents();
	void DeleteComponent(Component *component);
	void SerializeComponent(Component *component, IAssetStreamIO& stream);
	Component *DeserializeComponent(IAssetStreamIO& stream);
private:
	long m_deadAddedLast;
	long m_deadAddedNext;
	std::map<std::string, std::vector<Component *>> m_components;
	std::map<std::string, std::vector<Component *>> m_deadComponents;
	std::queue<Component *> m_toKill;
	PrototypeManager *m_pManager;
};

class EntityManager 
{
public:
	EntityManager(PrototypeManager *pManager, ComponentManager *cManager) : m_pManager(pManager), m_cManager(cManager) { }
	Entity CreateEntity(std::vector<std::string> components);
	Component *GetComponent(EntityId id, std::string component);
	std::vector<Component *>& GetComponents(EntityId id);
	void KillEntity(EntityId id);

	void AddComponent(EntityId id, std::string component);
	void RemoveComponent(EntityId id, std::string component);

	void SerializeEntity(EntityId id, IAssetStreamIO& stream);
	std::vector<Entity> LoadEntities(int count, IAssetStreamIO& stream);

	int GetEntityCount();
private:
	ComponentManager *m_cManager;
	PrototypeManager *m_pManager;
	std::map<EntityId, std::vector<Component *>> m_entities;
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
	template<class T>
	T *GetComponent() { return static_cast<T *>(m_manager->GetComponent(m_id, ComponentDescriptor<T>::GetName())); }
	void Kill() { m_manager->KillEntity(m_id); }
	bool IsNull() { return m_id == ""; }
private:
	EntityId m_id;
	EntityManager *m_manager;
};

class Component
{
public:
	Entity Owner;
	std::string InternalName;
};

class EventManager
{
public:
	std::vector<IEvent *> ListenEvents(std::string system);
	void SubscribeEvent(std::string system, std::string name);
	void PushEvent(std::string name, IEvent *event);
private:
	std::map<std::string, std::vector<std::string>> m_subscribers;
	std::map<std::string, std::vector<IEvent *>> m_eventsPerSubscriber;
};

class ISystem
{
public:
	virtual void InitializeSystem(EntityManager *eManager, ComponentManager *cManager, EventManager *evManager, PrototypeManager *pManager) = 0;
	virtual void Update(float deltaTime) = 0;
	virtual std::string GetName() = 0;
	virtual std::vector<std::string> GetSystemsBefore() = 0;
	virtual std::vector<std::string> GetSystemsAfter() = 0;
	virtual bool IsMainThread() = 0;

	inline void SetEnabled(bool enabled) { m_enabled = enabled; }
	inline bool IsEnabled() { return m_enabled; }

	int Group;
private:
	bool m_enabled;
};

class SystemManager
{
public:
	SystemManager(EntityManager *eManager, ComponentManager *cManager, EventManager *evManager, PrototypeManager *pManager) :
		m_eManager(eManager), m_cManager(cManager), m_evManager(m_evManager), m_pManager(pManager) { }
	~SystemManager();
	void AddSystem(ISystem *system);
	ISystem *GetSystem(std::string name);
	void InitializeSystems();
	void UpdateSystems(float deltaTime, int group=-1);
	inline std::vector<int> GetMainThreadGroups() { return m_mainThreadGroups; }
	inline std::vector<int> GetAnyThreadGroups() { return m_anyThreadGroups; }
private:
	std::vector<int> m_mainThreadGroups;
	std::vector<int> m_anyThreadGroups;

	std::map<std::string, ISystem *> m_systems;
	std::vector<std::vector<ISystem *>> m_systemGroups;
	EntityManager *m_eManager;
	ComponentManager *m_cManager;
	EventManager *m_evManager;
	PrototypeManager *m_pManager;
};