#include "Precompiled.h"
#include "World.h"
#include "Core/Entity.h"

World::World() {

}

World::~World() {

}

void World::update(float dt) {

}

void World::addEntity(const std::string &name, Entity *ent) {
	mEntities[name] = ent;
}

void World::removeEntity(Entity *ent) {
	mEntities.erase(ent->getOwner());
}