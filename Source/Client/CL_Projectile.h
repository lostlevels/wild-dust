#pragma once

#include "CL_PhysicsEntity.h"
#include "Core/Clock.h"

class Texture;

class CL_Projectile : public CL_PhysicsEntity {
public:
	CL_Projectile(Client *client);
	virtual ~CL_Projectile();

	virtual void readFromStream(const BitStream &stream);

	virtual void draw();

private:
	Vec2 mPosition;
	Texture *mBoomerang;
};
