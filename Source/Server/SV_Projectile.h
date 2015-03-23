#pragma once

#include "SV_PhysicsEntity.h"

class SV_Projectile : public SV_PhysicsEntity {
public:
	SV_Projectile(Server *server);

	virtual void writeToStream(BitStream &stream);

	virtual void update(float dt);
};
