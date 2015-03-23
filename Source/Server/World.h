#pragma once

#include "Core/BitStream.h"
#include "Shared/Protocol.h"
#include "Shared/EntityTypes.h"

class Server;
class SV_Entity;

typedef std::function<SV_Entity*(Server*)> CreateServerEntityFunc;

class ServerWorld {
public:
	ServerWorld(Server *server);
	~ServerWorld();

	void registerEntityType(EntityType type, CreateServerEntityFunc func);

	void writeToSnapshot(BitStream &snapshot);

	SV_Entity *spawnEntity(EntityType type);
	template<class T> T *spawnEntityTyped(EntityType type);
	EntityID findIDByEntity(SV_Entity *entity);
	SV_Entity *findEntityByID(EntityID id);
	void destroyEntity(SV_Entity *entity);
	void scheduleDestroyEntity(SV_Entity *entity);
	void destroyScheduledEntities();
	void deleteRemovedEntities();
	void deleteAllEntities();

	void update(float dt);

private:
	Server *mServer;
	std::vector<SV_Entity*> mEntities;
	std::vector<SV_Entity*> mRemovedEntities;
	std::vector<SV_Entity*> mEntitiesScheduledForDestruction;
	std::map<EntityID, SV_Entity*> mIDToEntityMap;
	std::map<SV_Entity*, EntityID> mEntityToIDMap;
	std::map<EntityType, CreateServerEntityFunc> mCreateFuncs;
};

template<class T> T *ServerWorld::spawnEntityTyped(EntityType type) {
	return reinterpret_cast<T*>(spawnEntity(type));
}
