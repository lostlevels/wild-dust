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
	mCowboyAnimSet.mAnimSheet = new AnimationSheet();
	mCowboyAnimSet.mAnimSheet->loadFromFile("../Content/Animations/Cowboy.xml");
	mCowboyAnimSet.mIdleAnim = mCowboyAnimSet.mAnimSheet->findAnimation("Idle");
	mCowboyAnimSet.mWalkAnim = mCowboyAnimSet.mAnimSheet->findAnimation("Walk");
	mCowboyAnimSet.mJumpAnim = mCowboyAnimSet.mAnimSheet->findAnimation("Jump");
	mCowboyAnimSet.mShootAnim = mCowboyAnimSet.mAnimSheet->findAnimation("Shoot");
	mCowboyAnimSet.mAnimRenderer = new AnimationRenderer(mCowboyAnimSet.mAnimSheet, client->getRenderer());

	mBanditAnimSet.mAnimSheet = new AnimationSheet();
	mBanditAnimSet.mAnimSheet->loadFromFile("../Content/Animations/Bandit.xml");
	mBanditAnimSet.mIdleAnim = mBanditAnimSet.mAnimSheet->findAnimation("Idle");
	mBanditAnimSet.mWalkAnim = mBanditAnimSet.mAnimSheet->findAnimation("Walk");
	mBanditAnimSet.mJumpAnim = mBanditAnimSet.mAnimSheet->findAnimation("Jump");
	mBanditAnimSet.mShootAnim = mBanditAnimSet.mAnimSheet->findAnimation("Shoot");
	mBanditAnimSet.mAnimRenderer = new AnimationRenderer(mBanditAnimSet.mAnimSheet, client->getRenderer());

	mMovement = new PlayerMovement(getPhysicsObject());

	mLookingLeft = false;

	mTeam = TEAM_COWBOYS;
}

CL_Player::~CL_Player() {
	delete mMovement;
	
	freeCharacterAnimationSet(&mBanditAnimSet);
	delete mBanditAnimSet.mAnimRenderer;

	freeCharacterAnimationSet(&mCowboyAnimSet);
	delete mCowboyAnimSet.mAnimRenderer;
}

void CL_Player::readFromStream(const BitStream &stream) {
	CL_PhysicsEntity::readFromStream(stream);

	PlayerState oldState = mState;

	mTeam = (Team)stream.readU8();
	mState = (PlayerState)stream.readU8();
	mLookingLeft = stream.readBool();
	getCurrentAnim()->setCurrentFrameIndex(stream.readU16());

	if (mState != oldState) {
		if (mState == PLAYER_SHOOTING) {
			getCurrentAnimSet()->mShootAnim->reset();
		}
	}
}

void CL_Player::update(float dt) {
	getCurrentAnim()->animate(dt);
}

void CL_Player::draw() {
	AnimationSheet *currAnimSheet = getCurrentAnimSet()->mAnimSheet;
	Animation *currAnim = getCurrentAnim();
	currAnim->setFlipX(mLookingLeft);
	getCurrentAnimSet()->mAnimRenderer->render(
		currAnim,
		getPhysicsObject()->getPosition(),
		Vec2(currAnimSheet->getFrameWidth(), currAnimSheet->getFrameHeight()),
		Color(1.0f));
}

CharacterAnimationSet *CL_Player::getCurrentAnimSet() {
	if (mTeam == TEAM_COWBOYS) {
		return &mCowboyAnimSet;
	}
	else if (mTeam == TEAM_BANDITS) {
		return &mBanditAnimSet;
	}
}

Animation *CL_Player::getCurrentAnim() {
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

Vec2 CL_Player::ICameraTarget_getPosition() const {
	return getPhysicsObject()->getPosition();
}

Vec2 CL_Player::ICameraTarget_getSize() const {
	return Vec2(16, 32);
}
