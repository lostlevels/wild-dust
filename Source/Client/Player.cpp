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
	
	mIdleAnimation = mAnimSheet->createAnimation("Idle", { 0 });
	mWalkAnimation = mAnimSheet->createAnimation("Walk", { 1, 2, 3, 4 });
	mShootAnimation = mAnimSheet->createAnimation("Shoot", { 9, 10, 11 });
}

CL_Player::~CL_Player() {
	delete mAnimSheet;
}

void CL_Player::readFromStream(const BitStream &stream) {
	mPosition.x = stream.readFloat();
	mPosition.y = stream.readFloat();
}

void CL_Player::update(float dt) {
	mWalkAnimation->animate(dt);
}

void CL_Player::draw() {
	mWalkAnimation->draw(mPosition, Vec2(16, 32), Color(1.0f));
}

Vec2 CL_Player::ICameraTarget_getPosition() const {
	return mPosition;
}

Vec2 CL_Player::ICameraTarget_getSize() const {
	return Vec2(16, 16);
}
