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
	mCowboyAnimSet.mAnimSheet = new AnimationSheet();
	mCowboyAnimSet.mAnimSheet->loadFromFile("../Content/Animations/Cowboy.xml");
	mCowboyAnimSet.mIdleAnim = mCowboyAnimSet.mAnimSheet->findAnimation("Idle");
	mCowboyAnimSet.mWalkAnim = mCowboyAnimSet.mAnimSheet->findAnimation("Walk");
	mCowboyAnimSet.mJumpAnim = mCowboyAnimSet.mAnimSheet->findAnimation("Jump");
	mCowboyAnimSet.mShootAnim = mCowboyAnimSet.mAnimSheet->findAnimation("Shoot");
	mCowboyAnimSet.mAnimRenderer = NULL;

	mBanditAnimSet.mAnimSheet = new AnimationSheet();
	mBanditAnimSet.mAnimSheet->loadFromFile("../Content/Animations/Bandit.xml");
	mBanditAnimSet.mIdleAnim = mBanditAnimSet.mAnimSheet->findAnimation("Idle");
	mBanditAnimSet.mWalkAnim = mBanditAnimSet.mAnimSheet->findAnimation("Walk");
	mBanditAnimSet.mJumpAnim = mBanditAnimSet.mAnimSheet->findAnimation("Jump");
	mBanditAnimSet.mShootAnim = mBanditAnimSet.mAnimSheet->findAnimation("Shoot");
	mBanditAnimSet.mAnimRenderer = NULL;

	mMovement = new PlayerMovement(getPhysicsObject());

	mState = PLAYER_IDLE;
	mLookingLeft = true;

	mTeam = TEAM_BANDITS;
}

SV_Player::~SV_Player() {
	delete mMovement;
	freeCharacterAnimationSet(&mBanditAnimSet);
	freeCharacterAnimationSet(&mCowboyAnimSet);
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
	stream.writeU8(mTeam);
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


CharacterAnimationSet *SV_Player::getCurrentAnimSet() {
	if (mTeam == TEAM_COWBOYS) {
		return &mCowboyAnimSet;
	}
	else if (mTeam == TEAM_BANDITS) {
		return &mBanditAnimSet;
	}
	return NULL;
}

Animation *SV_Player::getCurrentAnim() {
	switch (mState) {
	case PLAYER_IDLE:
		return getCurrentAnimSet()->mIdleAnim;

	case PLAYER_MOVING:
		return getCurrentAnimSet()->mWalkAnim;

	case PLAYER_JUMPING:
		return getCurrentAnimSet()->mJumpAnim;

	case PLAYER_SHOOTING:
		return getCurrentAnimSet()->mShootAnim;
	}
	return NULL;
}

