#pragma once
#include <string>
#include <map>

class Entity;

class SceneConfiguration
{
public:
	SceneConfiguration();
	~SceneConfiguration();
	Entity *GetEntityPrototype(std::string name);
protected:
	template<class T>
	void AddEntityPrototype()
	{
		Entity *entity = dynamic_cast<Entity *>(new T);
		_AddEntityPrototypeWithName(entity);
	}
private:
	void _AddEntityPrototypeWithName(Entity *entity);
	std::map<std::string, Entity *> m_prototypes;
};

