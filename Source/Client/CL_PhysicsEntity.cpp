#include "Precompiled.h"
#include "CL_PhysicsEntity.h"
#include "Client.h"
#include "Physics/Physics.h"
#include "Physics/PhysicsObject.h"

CL_PhysicsEntity::CL_PhysicsEntity(Client *client, bool canMove) : CL_Entity(client) {
	mPosition = Vec2(0.0f, 0.0f);
	mVelocity = Vec2(0.0f, 0.0f);
	mCanJump = false;
}

CL_PhysicsEntity::~CL_PhysicsEntity() {
}

void CL_PhysicsEntity::readFromStream(const BitStream &stream) {
	mPosition.x = stream.readFloat();
	mPosition.y = stream.readFloat();
	mVelocity.x = stream.readFloat();
	mVelocity.y = stream.readFloat();
	mCanJump = stream.readBool();
}
