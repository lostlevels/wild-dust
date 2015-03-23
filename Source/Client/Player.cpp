#include "Precompiled.h"
#include "Player.h"
#include "Client.h"
#include "Renderer.h"
#include "Texture.h"
#include "SpriteBatcher.h"

CL_Player::CL_Player(Client *client) : CL_Entity(client) {
	mGrass = client->getRenderer()->getTexture("../Content/Textures/Characters/Cowboy.png");
}

CL_Player::~CL_Player() {
	mGrass->decrementRefs();
}

void CL_Player::readFromStream(const BitStream &stream) {
	mPosition.x = stream.readFloat();
	mPosition.y = stream.readFloat();
}

void CL_Player::update(float dt) {

}

void CL_Player::draw() {
	Renderer *renderer = mClient->getRenderer();
	SpriteBatcher *batcher = renderer->getSpriteBatcher(mGrass, BLEND_ALPHA);
	batcher->addSprite(mPosition, Color(1.0f));
}

Vec2 CL_Player::ICameraTarget_getPosition() const {
	return mPosition;
}

Vec2 CL_Player::ICameraTarget_getSize() const {
	return Vec2(mGrass->getWidth(), mGrass->getHeight());
}
