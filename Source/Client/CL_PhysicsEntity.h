#pragma once

#include "CL_Entity.h"

class CL_PhysicsEntity : public CL_Entity {
public:
	CL_PhysicsEntity(Client *client, bool canMove);
	virtual ~CL_PhysicsEntity();

	virtual void readFromStream(const BitStream &stream);

protected:
	Vec2 mPosition;
	Vec2 mVelocity;
	bool mCanJump;
};
