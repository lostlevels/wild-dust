#pragma once

#include <map>
#include <string>

class Entity;

class CORE_API World {
public:
	World();
	virtual ~World();

	virtual void update(float gameTime, float dt);

	Entity *scheduleAddEntity(const std::string &name, Entity *ent);
	Entity *scheduleAddEntity(Entity *ent);

	virtual void scheduleDeleteEntity(Entity *ent);
	virtual void scheduleDeleteEntity(const std::string &id);

	const std::map<std::string, Entity*> &getEntities() const { return mEntities; }
	Entity *getEntity(const std::string &name) const;

protected:
	// id => entity
	std::map<std::string, Entity*> mEntities;

	void removeEntity(Entity *ent);
private:
	std::map<std::string, Entity*> mDeleteMap;
	std::map<std::string, Entity*> mAddMap;

	void deleteScheduledEntities();
	void addScheduledEntities();

	Entity *addEntity(const std::string &name, Entity *ent);
	Entity *addEntity(Entity *ent);

	World(const World&) = delete;
	const World& operator = (const World&) = delete;
};
