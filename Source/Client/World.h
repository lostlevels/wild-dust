#pragma once

#include "Core/BitStream.h"
#include "Shared/Protocol.h"

class Client;
class CL_Entity;

typedef std::function<CL_Entity*(Client*)> CreateClientEntityFunc;

class ClientWorld {
public:
	ClientWorld(Client *client);
	~ClientWorld();

	void registerEntityType(const std::string &typeName, CreateClientEntityFunc func);

	EntityID findIDByEntity(CL_Entity *entity);
	CL_Entity *findEntityByID(EntityID id);
	void deleteAllEntities();

	void readFromSnapshot(const BitStream &snapshot);
	void update(float dt);

	void draw();

private:
	void addEntity(EntityID id, CL_Entity *entity);

private:
	Client *mClient;
	std::map<std::string, CreateClientEntityFunc> mCreateFuncs;
	std::vector<CL_Entity*> mEntities;
	std::map<EntityID, CL_Entity*> mIDToEntityMap;
	std::map<CL_Entity*, EntityID> mEntityToIDMap;
};