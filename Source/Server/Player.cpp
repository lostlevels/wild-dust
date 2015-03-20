#include "Precompiled.h"
#include "Player.h"

SV_Player::SV_Player(Server *server) : SV_Entity(server) {

}

void SV_Player::update(float dt) {
}

void SV_Player::writeToStream(BitStream &stream) {
	stream.writeAny<Vec2>(mPosition);
}