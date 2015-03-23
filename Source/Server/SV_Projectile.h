#pragma once

#include "SV_Entity.h"

class SV_Projectile : public SV_Entity {
public:
	SV_Projectile(Server *server);

	virtual void writeToStream(BitStream &stream);

	virtual void update(float dt);

	Vec2 mPosition;
};
