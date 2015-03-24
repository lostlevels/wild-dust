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

void World::update(float dt) {

}

Entity *World::addEntity(const std::string &name, Entity *ent) {
	mEntities[name] = ent;
	return ent;
}

void World::removeEntity(Entity *ent) {
	mEntities.erase(ent->getId());
}

void World::scheduleDeleteEntity(Entity *ent) {
	// TODO:
}

Entity *World::getEntity(const std::string &name) const {
	auto kv = mEntities.find(name);
	return kv != mEntities.end() ? kv->second : nullptr;
}