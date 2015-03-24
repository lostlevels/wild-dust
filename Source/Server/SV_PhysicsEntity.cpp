#include "Precompiled.h"
#include "SV_PhysicsEntity.h"
#include "Server.h"
#include "Physics/Physics.h"
#include "Physics/PhysicsObject.h"

SV_PhysicsEntity::SV_PhysicsEntity(Server *server, bool canMove) : SV_Entity(server) {
	mPhysObject = server->getPhysics()->createObject(canMove ? PHYSICS_DYNAMIC : PHYSICS_STATIC);
	mPhysObject->setUserData(this);
}

SV_PhysicsEntity::~SV_PhysicsEntity() {
	delete mPhysObject;
}

void SV_PhysicsEntity::writeToStream(BitStream &stream) {
	Vec2 pos = getPhysicsObject()->getPosition();
	stream.writeFloat(pos.x);
	stream.writeFloat(pos.y);
	
	Vec2 vel = getPhysicsObject()->getVelocity();
	stream.writeFloat(vel.x);
	stream.writeFloat(vel.y);
	
	stream.writeFloat(getPhysicsObject()->getFriction());

	stream.writeU16(getPhysicsObject()->getWidth());
	stream.writeU16(getPhysicsObject()->getHeight());
}
