#include "Precompiled.h"
#include "World.h"
#include "Entity.h"

ServerWorld::ServerWorld() {

}

ServerWorld::~ServerWorld() {
	deleteAllEntities();
}

void ServerWorld::deleteAllEntities() {
	for (SV_Entity *entity : mEntities) {
		delete entity;
	}
	mEntities.clear();
}

void ServerWorld::update(float dt) {
	for (SV_Entity *entity : mEntities) {
		entity->update(dt);
	}
}
