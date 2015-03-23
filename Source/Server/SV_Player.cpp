#include "Precompiled.h"
#include "SV_Player.h"
#include "Server.h"
#include "World.h"
#include "SV_Projectile.h"

#define PLAYER_WIDTH 32
#define PLAYER_HEIGHT 64

#define PLAYER_SHOOT_COOLDOWN 0.5f

SV_Player::SV_Player(Server *server) : SV_PhysicsEntity(server, true) {
	setBox(32, 64);
	mState = PLAYER_IDLE;
	mLookingLeft = true;
}

void SV_Player::update(float dt) {
	if (mState == PLAYER_SHOOTING) {
		if (mShootTimer.getElapsedSeconds() >= PLAYER_SHOOT_COOLDOWN) {
			mState = PLAYER_IDLE;
		}
	}
	else {
		Vec2 vel = getVelocity();
		if (vel.y != 0) {
			mState = PLAYER_JUMPING;
		}
		else {
			if (vel.x != 0) {
				mState = PLAYER_MOVING;
			}
			else {
				mState = PLAYER_IDLE;
			}
		}
	}
}

void SV_Player::writeToStream(BitStream &stream) {
	SV_PhysicsEntity::writeToStream(stream);
	stream.writeU8(mState);
	stream.writeBool(mLookingLeft);
}

void SV_Player::moveLeft() {
	Vec2 vel = getVelocity();
	vel.x = -300;
	setVelocity(vel);
	mLookingLeft = true;
}

void SV_Player::moveRight() {
	Vec2 vel = getVelocity();
	vel.x = 300;
	setVelocity(vel);
	mLookingLeft = false;
}

void SV_Player::shoot() {
	if (mState == PLAYER_SHOOTING) {
		return;
	}
	mShootTimer.reset();
	mState = PLAYER_SHOOTING;

	Vec2 projOffset(0.0f, getHeight() / 2 + 5);
	if (mLookingLeft) {
		projOffset.x = -5;
	}
	else {
		projOffset.x = getWidth() + 5;
	}

	SV_Projectile *proj = mServer->getWorld()->spawnEntityTyped<SV_Projectile>(ENTITY_PROJECTILE);
	proj->setPosition(getPosition() + projOffset);
	proj->mMovingLeft = mLookingLeft;
	proj->mFiredBy = this;
}

void SV_Player::jump() {
	bool canJump = false;

	for (b2ContactEdge *edge = getBody()->GetContactList(); edge; edge = edge->next) {
		b2Contact *contact = edge->contact;
		if (contact->IsTouching() == false) {
			continue;
		}

		if (contact->GetManifold()->localNormal.y == -1) {
			canJump = true;
			break;
		}
	}

	if (canJump) {
		Vec2 vel = getVelocity();
		vel.y -= 180;
		setVelocity(vel);
	}
}
