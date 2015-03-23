#include "Precompiled.h"
#include "World.h"
#include "Entity.h"

ServerWorld::ServerWorld(Server *server) {
	mServer = server;
}

ServerWorld::~ServerWorld() {
	deleteAllEntities();
}

void ServerWorld::registerEntityType(const std::string &typeName, CreateServerEntityFunc func) {
	auto result = mCreateFuncs.find(typeName);
	if (result == mCreateFuncs.end()) {
		mCreateFuncs.insert({ typeName, func });
	}
}

void ServerWorld::writeToSnapshot(BitStream &snapshot) {
	snapshot.writeU32(mEntities.size());
	for (SV_Entity *entity : mEntities) {
		snapshot.writeAny<EntityID>(entity->mID);
		snapshot.writeString(entity->mTypeName);
		assert(entity->mTypeName.size() > 0);
		entity->writeToStream(snapshot);
	}

	snapshot.writeU32(mRemovedEntities.size());
	for (SV_Entity *entity : mRemovedEntities) {
		snapshot.writeAny<EntityID>(entity->mID);
	}
}

SV_Entity *ServerWorld::spawnEntity(const std::string &typeName) {
	auto result = mCreateFuncs.find(typeName);
	if (result == mCreateFuncs.end()) {
		return NULL;
	}

	SV_Entity *entity = result->second(mServer);
	mEntities.push_back(entity);

	static uint32_t entityID = 0;
	++entityID;

	entity->mTypeName = typeName;
	entity->mID = entityID;

	mIDToEntityMap.insert({ entityID, entity });
	mEntityToIDMap.insert({ entity, entityID });

	return entity;
}

void ServerWorld::destroyEntity(SV_Entity *entity) {
	for (auto it = mEntities.begin(); it != mEntities.end(); ++it) {
		if (*it == entity) {
			mEntities.erase(it);

			auto it1 = mEntityToIDMap.find(entity);
			mEntityToIDMap.erase(it1);

			auto it2 = mIDToEntityMap.find(entity->mID);
			mIDToEntityMap.erase(it2);

			mRemovedEntities.push_back(entity);

			break;
		}
	}
}

EntityID ServerWorld::findIDByEntity(SV_Entity *entity) {
	auto result = mEntityToIDMap.find(entity);
	if (result != mEntityToIDMap.end()) {
		return result->second;
	}
	return 0;
}

SV_Entity *ServerWorld::findEntityByID(EntityID id) {
	auto result = mIDToEntityMap.find(id);
	if (result != mIDToEntityMap.end()) {
		return result->second;
	}
	return NULL;
}

void ServerWorld::deleteRemovedEntities() {
	for (SV_Entity *entity : mRemovedEntities) {
		delete entity;
	}
	mRemovedEntities.clear();
}

void ServerWorld::deleteAllEntities() {
	deleteRemovedEntities();

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
