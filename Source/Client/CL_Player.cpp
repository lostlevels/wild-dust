#include "Precompiled.h"
#include "CL_Player.h"
#include "Client.h"
#include "Renderer.h"
#include "Texture.h"
#include "SpriteBatcher.h"
#include "Shared/AnimationSheet.h"
#include "Physics/PhysicsObject.h"
#include "Physics/PlayerMovement.h"
#include "AnimationRenderer.h"

CL_Player::CL_Player(Client *client) : CL_PhysicsEntity(client, true) {
	mAnimSheet = new AnimationSheet();
	mAnimSheet->loadFromFile("../Content/Animations/Cowboy.xml");
	mIdleAnimation = mAnimSheet->findAnimation("Idle");
	mWalkAnimation = mAnimSheet->findAnimation("Walk");
	mJumpAnimation = mAnimSheet->findAnimation("Jump");
	mShootAnimation = mAnimSheet->findAnimation("Shoot");
	mAnimRenderer = new AnimationRenderer(mAnimSheet, client->getRenderer());

	mMovement = new PlayerMovement(getPhysicsObject());

	mLookingLeft = false;
}

CL_Player::~CL_Player() {
	delete mMovement;
	delete mAnimRenderer;
	delete mAnimSheet;
}

void CL_Player::readFromStream(const BitStream &stream) {
	CL_PhysicsEntity::readFromStream(stream);

	PlayerState oldState = mState;

	mState = (PlayerState)stream.readU8();
	mLookingLeft = stream.readBool();
	getCurrentAnim()->setCurrentFrameIndex(stream.readU16());

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
	Animation *currAnim = getCurrentAnim();
	currAnim->setFlipX(mLookingLeft);
	mAnimRenderer->render(currAnim, getPhysicsObject()->getPosition(), Vec2(mAnimSheet->getFrameWidth(), mAnimSheet->getFrameHeight()), Color(1.0f));
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
	return Vec2(16, 32);
}
