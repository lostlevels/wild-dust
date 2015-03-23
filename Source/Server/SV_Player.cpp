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
}

void SV_Player::moveLeft() {
	Vec2 vel = getVelocity();
	vel.x -= 100;
	setVelocity(vel);
}

void SV_Player::moveRight() {
	Vec2 vel = getVelocity();
	vel.x += 100;
	setVelocity(vel);
}

void SV_Player::shoot() {
	if (mState == PLAYER_SHOOTING) {
		return;
	}
	mShootTimer.reset();
	mState = PLAYER_SHOOTING;

	SV_Projectile *proj = mServer->getWorld()->spawnEntityTyped<SV_Projectile>("Projectile");
	proj->setPosition(getPosition() + Vec2(getWidth() + 1, getHeight() / 2 + 5));
}

void SV_Player::jump() {
	Vec2 vel = getVelocity();
	vel.y -= 100;
	setVelocity(vel);
}
