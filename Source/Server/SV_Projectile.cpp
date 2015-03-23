#include "Precompiled.h"
#include "SV_Projectile.h"

SV_Projectile::SV_Projectile(Server *server) : SV_Entity(server) {

}

void SV_Projectile::writeToStream(BitStream &stream) {
	stream.writeAny<Vec2>(mPosition);
}

void SV_Projectile::update(float dt) {
	mPosition += Vec2(1.0f, 0.0f) * dt * 20.0f;
}
