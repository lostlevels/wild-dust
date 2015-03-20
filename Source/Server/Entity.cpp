#include "Precompiled.h"
#include "Entity.h"

SV_Entity::SV_Entity(Server *server) {
	mServer = server;
}

SV_Entity::~SV_Entity() {

}

void SV_Entity::writeToStream(BitStream &stream) {

}

void SV_Entity::readFromStream(const BitStream &stream) {

}

void SV_Entity::update(float dt) {

}
