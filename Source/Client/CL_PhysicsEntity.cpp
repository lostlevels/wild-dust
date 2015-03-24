#include "Precompiled.h"
#include "CL_PhysicsEntity.h"
#include "Client.h"
#include "Physics/Physics.h"
#include "Physics/PhysicsObject.h"

CL_PhysicsEntity::CL_PhysicsEntity(Client *client, bool canMove) : CL_Entity(client) {
	mPhysObject = client->getPhysics()->createObject(canMove ? PHYSICS_DYNAMIC : PHYSICS_STATIC);
}

CL_PhysicsEntity::~CL_PhysicsEntity() {
	delete mPhysObject;
}

void CL_PhysicsEntity::readFromStream(const BitStream &stream) {
	Vec2 position;
	position.x = stream.readFloat();
	position.y = stream.readFloat();

	Vec2 velocity;
	velocity.x = stream.readFloat();
	velocity.y = stream.readFloat();

	float friction = stream.readFloat();

	int width = stream.readU16();
	int height = stream.readU16();

	PhysicsObject *physObj = getPhysicsObject();

	if (width != physObj->getWidth() || height != physObj->getHeight()) {
		physObj->setBox(width, height);
	}

	physObj->setPosition(position);
	physObj->setVelocity(velocity);
	physObj->setFriction(friction);
}
