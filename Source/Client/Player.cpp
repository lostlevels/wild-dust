#include "Precompiled.h"
#include "Player.h"
#include "Client.h"
#include "Renderer.h"
#include "Texture.h"
#include "SpriteBatcher.h"
#include "AnimationSheet.h"

CL_Player::CL_Player(Client *client) : CL_Entity(client) {
	mAnimSheet = new AnimationSheet(client->getRenderer());
	mAnimSheet->loadFromFile("../Content/Textures/Characters/Cowboy.png", 18, 32);
	
	mIdleAnimation = mAnimSheet->createAnimation("Idle", { 12 });
	mIdleAnimation->setLoopCount(0);
	
	mWalkAnimation = mAnimSheet->createAnimation("Walk", { 6, 7, 8, 9 });
	
	mJumpAnimation = mAnimSheet->createAnimation("Jump", { 5 });

	mShootAnimation = mAnimSheet->createAnimation("Shoot", { 9, 10, 11, 12, 13 });
	mShootAnimation->setLoopCount(1);
}

CL_Player::~CL_Player() {
	delete mAnimSheet;
}

void CL_Player::readFromStream(const BitStream &stream) {
	PlayerState oldState = mState;

	mPosition.x = stream.readFloat();
	mPosition.y = stream.readFloat();
	mSize.x = stream.readU16();
	mSize.y = stream.readU16();
	mState = (PlayerState)stream.readU8();
	
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
	getCurrentAnim()->draw(mPosition, mSize, Color(1.0f));
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
	return mPosition;
}

Vec2 CL_Player::ICameraTarget_getSize() const {
	return mSize;
}
