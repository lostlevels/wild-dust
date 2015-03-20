#include "Precompiled.h"
#include "Entity.h"

CL_Entity::CL_Entity(Client *client) {
	mClient = client;
}

CL_Entity::~CL_Entity() {

}

void CL_Entity::readFromStream(const BitStream &stream) {

}

void CL_Entity::update(float dt){

}

void CL_Entity::draw() {

}
