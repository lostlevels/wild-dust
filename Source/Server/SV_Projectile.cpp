#include "Precompiled.h"
#include "SV_Projectile.h"
#include "Server.h"
#include "World.h"
#include "SV_Player.h"

#define PROJECTILE_VELOCITY 1000

SV_Projectile::SV_Projectile(Server *server) : SV_PhysicsEntity(server, true) {
	getPhysicsObject()->setBox(8, 8);
	mMovingLeft = false;
	mFiredBy = NULL;
}

void SV_Projectile::writeToStream(BitStream &stream) {
	SV_PhysicsEntity::writeToStream(stream);
}

void SV_Projectile::update(float dt) {
	bool shouldDestroy = false;

	std::vector<PhysicsCollision> cols = getPhysicsObject()->getCollisions();
	
	for (PhysicsCollision &col : cols) {
		SV_Entity *collider = (SV_Entity*)col.otherObject->getUserData();
		if (collider) {
			if (collider->getEntityType() == ENTITY_PLAYER) {
				if (collider != mFiredBy) {
					// TODO: Handle collision with other player
					shouldDestroy = true;
					break;
				}
			}
		}
		else {
			shouldDestroy = true;
			break;
		}
	}
	
	if (shouldDestroy) {
		// Collision occured, destroy self
		mServer->getWorld()->scheduleDestroyEntity(this);
		return;
	}

	getPhysicsObject()->setVelocity(Vec2(mMovingLeft ? -PROJECTILE_VELOCITY : PROJECTILE_VELOCITY, 0.0));
}
