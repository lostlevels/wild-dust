#include "Precompiled.h"
#include "CL_PhysicsEntity.h"

CL_PhysicsEntity::CL_PhysicsEntity(Client *client) : CL_Entity(client) {
	mPosition = Vec2(0.0f, 0.0f);
	mSize = Vec2(0.0f, 0.0f);
}

void CL_PhysicsEntity::readFromStream(const BitStream &stream) {
	mPosition.x = stream.readFloat();
	mPosition.y = stream.readFloat();
	mSize.x = (float)stream.readU16();
	mSize.y = (float)stream.readU16();
}
