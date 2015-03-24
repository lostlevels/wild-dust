#include "Precompiled.h"
#include "PlayerMovement.h"
#include "PhysicsObject.h"

PlayerMovement::PlayerMovement(PhysicsObject *playerObject) {
	mPlayerObject = playerObject;
}

void PlayerMovement::moveLeft() {
	Vec2 vel = mPlayerObject->getVelocity();
	vel.x = -300;
	mPlayerObject->setVelocity(vel);
}

void PlayerMovement::moveRight() {
	Vec2 vel = mPlayerObject->getVelocity();
	vel.x = 300;
	mPlayerObject->setVelocity(vel);
}

void PlayerMovement::jump() {
	bool canJump = false;

	std::vector<PhysicsCollision> cols = mPlayerObject->getCollisions();

	for (PhysicsCollision &col : cols) {
		if (col.edgeNormal.y == -1) {
			canJump = true;
			break;
		}
	}

	if (canJump) {
		Vec2 vel = mPlayerObject->getVelocity();
		vel.y = -375;
		mPlayerObject->setVelocity(vel);
	}
}
