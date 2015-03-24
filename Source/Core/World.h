#pragma once

#include <map>
#include <string>

class Entity;

class World {
public:
	World();
	virtual ~World();

	virtual void update(float dt);

	Entity *addEntity(const std::string &name, Entity *ent);
	void scheduleDeleteEntity(Entity *ent);

	const std::map<std::string, Entity*> &getEntities() const { return mEntities; }
	Entity *getEntity(const std::string &name) const;

protected:
	// id => entity
	std::map<std::string, Entity*> mEntities;

	void removeEntity(Entity *ent);
private:
	World(const World&) = delete;
	const World& operator = (const World&) = delete;
};
