#include "Precompiled.h"
#include "CL_Player.h"
#include "Client.h"
#include "Renderer.h"
#include "Texture.h"
#include "SpriteBatcher.h"
#include "AnimationSheet.h"
#include "Physics/PhysicsObject.h"
#include "Physics/PlayerMovement.h"

CL_Player::CL_Player(Client *client) : CL_PhysicsEntity(client, true) {
	mAnimSheet = new AnimationSheet(client->getRenderer());
	mAnimSheet->loadFromFile("../Content/Textures/Characters/Cowboy.png", 18, 32);
	
	mIdleAnimation = mAnimSheet->createAnimation("Idle", { 12 });
	mIdleAnimation->setLoopCount(0);
	
	mWalkAnimation = mAnimSheet->createAnimation("Walk", { 6, 7, 8, 9 });
	
	mJumpAnimation = mAnimSheet->createAnimation("Jump", { 5 });

	mShootAnimation = mAnimSheet->createAnimation("Shoot", { 9, 10, 11, 12, 13 });
	mShootAnimation->setLoopCount(1);
	mShootAnimation->setSpeed(10.0f);

	mMovement = new PlayerMovement(getPhysicsObject());

	mLookingLeft = false;
}

CL_Player::~CL_Player() {
	delete mMovement;
	delete mAnimSheet;
}

void CL_Player::readFromStream(const BitStream &stream) {
	CL_PhysicsEntity::readFromStream(stream);

	PlayerState oldState = mState;

	mState = (PlayerState)stream.readU8();
	mLookingLeft = stream.readBool();

	if (mState != oldState) {
		if (mState == PLAYER_SHOOTING) {
			mShootAnimation->reset();
		}
	}
}

void CL_Player::update(float dt) {
	getCurrentAnim()->animate(dt);
}

void CL_Player::draw() {
	Vec2 size(
		getPhysicsObject()->getWidth(),
		getPhysicsObject()->getHeight());

	Animation *currAnim = getCurrentAnim();
	currAnim->setFlipX(mLookingLeft);
	currAnim->draw(getPhysicsObject()->getPosition(), size, Color(1.0f));
}

Animation *CL_Player::getCurrentAnim() {
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

Vec2 CL_Player::ICameraTarget_getPosition() const {
	return getPhysicsObject()->getPosition();
}

Vec2 CL_Player::ICameraTarget_getSize() const {
	return Vec2(
		getPhysicsObject()->getWidth(),
		getPhysicsObject()->getHeight());
}
