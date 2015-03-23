#pragma once

#include <map>
#include <string>

class Entity;

class World {
public:
	World();
	virtual ~World();

	virtual void update(float dt);

	void addEntity(const std::string &name, Entity *ent);
	void removeEntity(Entity *ent);
private:
	std::map<std::string, Entity*> mEntities;
};
