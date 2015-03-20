#pragma once

#include "Core/BitStream.h"
#include "Shared/Protocol.h"

class Server;

class SV_Entity {
protected:
	SV_Entity(Server *server);
public:
	virtual ~SV_Entity();

	virtual void writeToStream(BitStream &stream);

	virtual void update(float dt);

protected:
	Server *mServer;

private:
	std::string mTypeName;
	EntityID mID;
	friend class ServerWorld;
};
