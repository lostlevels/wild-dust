#include "Precompiled.h"
#include "Projectile.h"
#include "Texture.h"
#include "Client.h"
#include "Renderer.h"

CL_Projectile::CL_Projectile(Client *client) : CL_Entity(client) {
	mBoomerang = client->getRenderer()->getTexture("../Content/Textures/Boomerang.png");
}

CL_Projectile::~CL_Projectile() {
	mBoomerang->decrementRefs();
}

void CL_Projectile::readFromStream(const BitStream &stream) {
	CL_Entity::readFromStream(stream);
	mPosition = stream.readAny<Vec2>();
}

void CL_Projectile::draw() {
	Recti source;
	source.x = 0;
	source.y = 0;
	source.w = mBoomerang->getWidth() / 5;
	source.h = mBoomerang->getHeight();

	mClient->getRenderer()->drawQuad(mBoomerang, mPosition, source, 1.0f, 0.0f);
}
