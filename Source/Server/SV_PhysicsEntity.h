#pragma once

#include "SV_Entity.h"
#include "Physics/PhysicsObject.h"

class SV_PhysicsEntity : public SV_Entity {
public:
	SV_PhysicsEntity(Server *server, bool canMove);
	virtual ~SV_PhysicsEntity();

	virtual void writeToStream(BitStream &stream);

	PhysicsObject *getPhysicsObject() { return mPhysObject; }

private:
	PhysicsObject *mPhysObject;
};
