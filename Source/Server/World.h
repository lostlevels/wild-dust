#pragma once

#include "Core/BitStream.h"
#include "Shared/Protocol.h"

class Server;
class SV_Entity;

typedef std::function<SV_Entity*(Server*)> CreateServerEntityFunc;

class ServerWorld {
public:
	ServerWorld(Server *server);
	~ServerWorld();

	void registerEntityType(const std::string &typeName, CreateServerEntityFunc func);

	void writeToSnapshot(BitStream &snapshot);

	SV_Entity *spawnEntity(const std::string &typeName);
	template<class T> T *spawnEntityTyped(const std::string &typeName);
	EntityID findIDByEntity(SV_Entity *entity);
	SV_Entity *findEntityByID(EntityID id);
	void destroyEntity(SV_Entity *entity);
	void deleteRemovedEntities();
	void deleteAllEntities();

	void update(float dt);

private:
	Server *mServer;
	std::vector<SV_Entity*> mEntities;
	std::vector<SV_Entity*> mRemovedEntities;
	std::map<EntityID, SV_Entity*> mIDToEntityMap;
	std::map<SV_Entity*, EntityID> mEntityToIDMap;
	std::map<std::string, CreateServerEntityFunc> mCreateFuncs;
};

template<class T> T *ServerWorld::spawnEntityTyped(const std::string &typeName) {
	return reinterpret_cast<T*>(spawnEntity(typeName));
}
