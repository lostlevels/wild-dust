#include "Precompiled.h"
#include "World.h"
#include "Entity.h"

ClientWorld::ClientWorld(Client *client) {
	mClient = client;
}

ClientWorld::~ClientWorld() {
	deleteAllEntities();
}

void ClientWorld::deleteAllEntities() {
	for (CL_Entity *entity : mEntities) {
		delete entity;
	}
	mEntities.clear();
}

void ClientWorld::update(float dt) {
	for (CL_Entity *entity : mEntities) {
		entity->update(dt);
	}
}
