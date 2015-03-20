#include "Precompiled.h"
#include "Player.h"
#include "Client.h"
#include "Renderer.h"
#include "Texture.h"

CL_Player::CL_Player(Client *client) : CL_Entity(client) {
	mGrass = client->getRenderer()->getTexture("../Content/Textures/Grass.jpg");
}

CL_Player::~CL_Player() {
	mGrass->decrementRefs();
}

void CL_Player::writeToStream(BitStream &stream) {
	
}

void CL_Player::readFromStream(const BitStream &stream) {
	mPosition = stream.readAny<Vec2>();
}

void CL_Player::update(float dt) {

}

void CL_Player::draw() {
	Renderer *renderer = mClient->getRenderer();
	renderer->drawQuad(mGrass, mPosition, 0.0f, 1.0f, 0.0f);
}
