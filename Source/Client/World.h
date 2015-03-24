#pragma once

#include "Core/BitStream.h"
#include "Shared/Protocol.h"
#include "Shared/EntityTypes.h"

class Client;
class CL_Entity;

typedef std::function<CL_Entity*(Client*)> CreateClientEntityFunc;

class ClientWorld {
public:
	ClientWorld(Client *client);
	~ClientWorld();

	void registerEntityType(EntityType type, CreateClientEntityFunc func);

	EntityID findIDByEntity(CL_Entity *entity);
	CL_Entity *findEntityByID(EntityID id);
	void deleteRemovedEntities();
	void deleteAllEntities();

	void readFromSnapshot(const BitStream &snapshot);
	void update(float dt);

	void draw();

	void addSky();

private:
	void addEntity(EntityID id, CL_Entity *entity);
	void removeEntity(EntityID, CL_Entity *entity);

private:
	Client *mClient;
	std::map<EntityType, CreateClientEntityFunc> mCreateFuncs;
	std::vector<CL_Entity*> mEntities;
	std::vector<CL_Entity*> mRemovedEntities;
	std::map<EntityID, CL_Entity*> mIDToEntityMap;
	std::map<CL_Entity*, EntityID> mEntityToIDMap;
};