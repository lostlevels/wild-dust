#include "Precompiled.h"
#include "SV_Player.h"
#include "Server.h"
#include "World.h"
#include "SV_Projectile.h"
#include "Physics/PhysicsObject.h"
#include "Physics/PlayerMovement.h"
#include "Shared/AnimationSheet.h"

#define PLAYER_WIDTH 32
#define PLAYER_HEIGHT 64

#define PLAYER_SHOOT_COOLDOWN 0.5f

SV_Player::SV_Player(Server *server) : SV_PhysicsEntity(server, true) {
	mAnimSheet = new AnimationSheet();
	mAnimSheet->loadFromFile("../Content/Animations/Cowboy.xml");
	mIdleAnimation = mAnimSheet->findAnimation("Idle");
	mWalkAnimation = mAnimSheet->findAnimation("Walk");
	mJumpAnimation = mAnimSheet->findAnimation("Jump");
	mShootAnimation = mAnimSheet->findAnimation("Shoot");

	mMovement = new PlayerMovement(getPhysicsObject());

	mState = PLAYER_IDLE;
	mLookingLeft = true;
}

SV_Player::~SV_Player() {
	delete mMovement;
	delete mAnimSheet;
}

void SV_Player::update(float dt) {
	if (mState == PLAYER_SHOOTING) {
		if (mShootTimer.getElapsedSeconds() >= PLAYER_SHOOT_COOLDOWN) {
			mState = PLAYER_IDLE;
		}
	}
	else {
		Vec2 vel = getPhysicsObject()->getVelocity();
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

	getCurrentAnim()->animate(dt);

	const AnimationFrame &frame = getCurrentAnim()->getCurrentFrame();
	getPhysicsObject()->setBox(frame.aabb);
}

void SV_Player::writeToStream(BitStream &stream) {
	SV_PhysicsEntity::writeToStream(stream);
	stream.writeU8(mState);
	stream.writeBool(mLookingLeft);
	stream.writeU16(getCurrentAnim()->getCurrentFrameIndex());
}

void SV_Player::shoot() {
	if (mState == PLAYER_SHOOTING) {
		return;
	}
	mShootTimer.reset();
	mState = PLAYER_SHOOTING;

	Vec2 projOffset(0.0f, getPhysicsObject()->GetBox().h / 2 + 5);
	if (mLookingLeft) {
		projOffset.x = -5;
	}
	else {
		projOffset.x = getPhysicsObject()->GetBox().w + 5;
	}

	SV_Projectile *proj = mServer->getWorld()->spawnEntityTyped<SV_Projectile>(ENTITY_PROJECTILE);
	proj->getPhysicsObject()->setPosition(getPhysicsObject()->getPosition() + projOffset);
	proj->mMovingLeft = mLookingLeft;
	proj->mFiredBy = this;
}

Animation *SV_Player::getCurrentAnim() {
	switch (mState) {
	case PLAYER_IDLE:
		return mIdleAnimation;

	case PLAYER_MOVING:
		return mWalkAnimation;

	case PLAYER_JUMPING:
		return mJumpAnimation;

	case PLAYER_SHOOTING:
		return mShootAnimation;
	}
	return NULL;
}
