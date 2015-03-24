#pragma once

#include "CL_Entity.h"

class PhysicsObject;

class CL_PhysicsEntity : public CL_Entity {
public:
	CL_PhysicsEntity(Client *client, bool canMove);
	virtual ~CL_PhysicsEntity();

	virtual void readFromStream(const BitStream &stream);

	PhysicsObject *getPhysicsObject() const { return mPhysObject; }

private:
	PhysicsObject *mPhysObject;
};
