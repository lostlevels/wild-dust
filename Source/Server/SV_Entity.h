#pragma once

#include "Core/BitStream.h"
#include "Shared/Protocol.h"
#include "Shared/EntityTypes.h"

class Server;

class SV_Entity {
protected:
	SV_Entity(Server *server);
public:
	virtual ~SV_Entity();

	virtual void writeToStream(BitStream &stream);

	virtual void update(float dt);

	EntityType getEntityType() const { return mType; }
	EntityID getEntityID() const { return mID; }

protected:
	Server *mServer;

private:
	EntityType mType;
	EntityID mID;
	friend class ServerWorld;
};
