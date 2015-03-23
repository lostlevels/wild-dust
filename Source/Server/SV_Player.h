#pragma once

#include "SV_PhysicsEntity.h"
#include "Core/Clock.h"
#include "Shared/PlayerState.h"

class SV_Player : public SV_PhysicsEntity {
public:
	SV_Player(Server *server);

	virtual void update(float dt);

	virtual void writeToStream(BitStream &stream);

	virtual void moveLeft();
	virtual void moveRight();

	virtual void shoot();
	virtual void jump();

protected:
	PlayerState mState;
	Clock mShootTimer;
};
