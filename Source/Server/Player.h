#pragma once

#include "Entity.h"

class SV_Player : public SV_Entity {
public:
	SV_Player(Server *server);

	virtual void update(float dt);

	virtual void writeToStream(BitStream &stream);

	virtual void moveLeft();
	virtual void moveRight();

	virtual void shoot();
	virtual void jump();

protected:
	b2Body *mPhysBody;
	b2Fixture *mPhysFixture;

private:
	void createPhysicsBody();
};
