#pragma once

#include "Core/BitStream.h"
#include "Shared/Protocol.h"

class Client;

class CL_Entity {
protected:
	CL_Entity(Client *client);
public:
	virtual ~CL_Entity();

	virtual void readFromStream(const BitStream &stream);

	virtual void update(float dt);

	virtual void draw();

protected:
	Client *mClient;

private:
	std::string mTypeName;
	EntityID mID;
	friend class ClientWorld;
};
