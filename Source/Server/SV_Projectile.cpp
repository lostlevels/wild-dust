#include "Precompiled.h"
#include "SV_Projectile.h"

SV_Projectile::SV_Projectile(Server *server) : SV_PhysicsEntity(server, true) {
	setBox(8, 8);
}

void SV_Projectile::writeToStream(BitStream &stream) {
	SV_PhysicsEntity::writeToStream(stream);
}

void SV_Projectile::update(float dt) {
	setVelocity(Vec2(500.0, 0.0));
}
