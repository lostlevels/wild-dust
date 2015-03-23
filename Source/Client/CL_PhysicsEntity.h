#pragma once

#include "CL_Entity.h"

class CL_PhysicsEntity : public CL_Entity {
public:
	CL_PhysicsEntity(Client *client);

	virtual void readFromStream(const BitStream &stream);

	const Vec2 &getPosition() const { return mPosition; }
	const Vec2 &getSize() const { return mSize; }

private:
	Vec2 mPosition;
	Vec2 mSize;
};
