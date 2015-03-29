#include "Precompiled.h"
#include "World.h"
#include "Core/Entity.h"

World::World() {

}

World::~World() {
	for (auto &kv : mEntities) {
		delete kv.second;
	}
	mEntities.clear();
}

void World::update(float gameTime, float dt) {
	for (auto &kv : mEntities) {
		kv.second->update(gameTime, dt);
	}

	addScheduledEntities();
	deleteScheduledEntities();
}

void World::addScheduledEntities() {
	for (auto &kv : mAddMap) {
		addEntity(kv.first, kv.second);
	}
	mAddMap.clear();
}

void World::deleteScheduledEntities() {
	for (auto &kv : mDeleteMap) {
		mEntities.erase(kv.first);
		delete kv.second;
	}
	mDeleteMap.clear();
}

Entity *World::addEntity(const std::string &name, Entity *ent) {
	mEntities[name] = ent;
	return ent;
}

Entity *World::addEntity(Entity *ent) {
	assert(!ent->getId().empty());
	return addEntity(ent->getId(), ent);
}

Entity *World::scheduleAddEntity(const std::string &name, Entity *ent) {
	assert(!ent->getId().empty());
	mAddMap[name] = ent;
	return ent;
}

Entity *World::scheduleAddEntity(Entity *ent) {
	return scheduleAddEntity(ent->getId(), ent);
}

void World::removeEntity(Entity *ent) {
	mEntities.erase(ent->getId());
}

void World::scheduleDeleteEntity(const std::string &id) {
	scheduleDeleteEntity(getEntity(id));
}

void World::scheduleDeleteEntity(Entity *ent) {
	if (ent) {
		mDeleteMap[ent->getId()] = ent;
	}
}

Entity *World::getEntity(const std::string &name) const {
	auto kv = mEntities.find(name);
	return kv != mEntities.end() ? kv->second : nullptr;
}