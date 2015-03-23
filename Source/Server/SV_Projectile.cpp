#include "Precompiled.h"
#include "SV_Projectile.h"
#include "Server.h"
#include "World.h"
#include "SV_Player.h"

#define PROJECTILE_VELOCITY 1000

SV_Projectile::SV_Projectile(Server *server) : SV_PhysicsEntity(server, true) {
	setBox(8, 8);
	mMovingLeft = false;
	mFiredBy = NULL;
}

void SV_Projectile::writeToStream(BitStream &stream) {
	SV_PhysicsEntity::writeToStream(stream);
}

void SV_Projectile::update(float dt) {
	bool shouldDestroy = false;

	for (const b2ContactEdge *contactEdge = getBody()->GetContactList(); contactEdge; contactEdge = contactEdge->next) {
		const b2Contact *contact = contactEdge->contact;
		if (!contact->IsTouching()) {
			continue;
		}

		SV_Entity *collider = (SV_Entity*)contact->GetFixtureA()->GetUserData();
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

	setVelocity(Vec2(mMovingLeft ? -PROJECTILE_VELOCITY : PROJECTILE_VELOCITY, 0.0));
}
