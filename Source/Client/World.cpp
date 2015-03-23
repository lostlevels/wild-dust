#include "Precompiled.h"
#include "World.h"
#include "CL_Entity.h"
#include "Shared/Protocol.h"

ClientWorld::ClientWorld(Client *client) {
	mClient = client;
}

ClientWorld::~ClientWorld() {
	deleteAllEntities();
}

void ClientWorld::registerEntityType(EntityType type, CreateClientEntityFunc func) {
	auto result = mCreateFuncs.find(type);
	if (result == mCreateFuncs.end()) {
		mCreateFuncs.insert({ type, func });
	}
}

CL_Entity *ClientWorld::findEntityByID(EntityID id) {
	auto result = mIDToEntityMap.find(id);
	if (result != mIDToEntityMap.end()) {
		return result->second;
	}
	return NULL;
}

EntityID ClientWorld::findIDByEntity(CL_Entity *entity) {
	auto result = mEntityToIDMap.find(entity);
	if (result != mEntityToIDMap.end()) {
		return result->second;
	}
	return 0;
}

void ClientWorld::deleteRemovedEntities() {
	for (CL_Entity *entity : mRemovedEntities) {
		delete entity;
	}
	mRemovedEntities.clear();
}

void ClientWorld::deleteAllEntities() {
	deleteRemovedEntities();

	for (CL_Entity *entity : mEntities) {
		delete entity;
	}
	mEntities.clear();
}

void ClientWorld::readFromSnapshot(const BitStream &snapshot) {
	uint32_t numEnts = snapshot.readU32();
	for (uint32_t i = 0; i < numEnts; ++i) {
		EntityID entityID = snapshot.readAny<EntityID>();
		EntityType entityType = (EntityType)snapshot.readAny<uint32_t>();

		CL_Entity *entity = findEntityByID(entityID);
		if (entity == NULL) {
			auto createFunc = mCreateFuncs.find(entityType);
			if (createFunc != mCreateFuncs.end()) {
				entity = createFunc->second(mClient);
				entity->mType = entityType;
				addEntity(entityID, entity);
			}
		}

		entity->readFromStream(snapshot);
	}

	uint32_t numRemovedEnts = snapshot.readU32();
	for (uint32_t i = 0; i < numRemovedEnts; ++i) {
		EntityID id = snapshot.readAny<EntityID>();
		CL_Entity *entity = findEntityByID(id);
		if (entity) {
			removeEntity(id, entity);
		}
	}
}

void ClientWorld::addEntity(EntityID id, CL_Entity *entity) {
	entity->mID = id;

	mIDToEntityMap.insert({ id, entity });
	mEntityToIDMap.insert({ entity, id });

	mEntities.push_back(entity);
}

void ClientWorld::removeEntity(EntityID id, CL_Entity *entity) {
	entity->mID = 0;

	mIDToEntityMap.erase(mIDToEntityMap.find(id));
	mEntityToIDMap.erase(mEntityToIDMap.find(entity));

	for (auto it = mEntities.begin(); it != mEntities.end(); ++it) {
		if (*it == entity) {
			mEntities.erase(it);
			break;
		}
	}

	mRemovedEntities.push_back(entity);
}

void ClientWorld::update(float dt) {
	for (CL_Entity *entity : mEntities) {
		entity->update(dt);
	}
}

void ClientWorld::draw() {
	for (CL_Entity *entity : mEntities) {
		entity->draw();
	}
}
