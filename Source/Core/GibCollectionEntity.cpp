#include "Precompiled.h"
#include "GibCollectionEntity.h"
#include "WorldCollision.h"
#include <glm/gtx/norm.hpp>

static float getRandom() {
	return (rand()%1000) / 1000.0f;
}

static float getSignedRandom() {
	return (rand()%2000) / 1000.0f - 1.0f;
}

GibCollectionEntity::GibCollectionEntity(WorldCollision *collider, const std::string &id, const EntityRepresentation &rep) :
	Entity(id, "gibs", "", SEND_NEVER, rep),
	mCollider(collider) {

	for (int i = 0; i < MAX_GIBS; ++i) {
		mFreeGibs[i] = i;
	}

	mNumFree = MAX_GIBS;
	mNumActive = 0;
}

GibCollectionEntity::~GibCollectionEntity() {

}

void GibCollectionEntity::update(float gameTime, float dt) {
	Entity::update(gameTime, dt);

	for (int i = 0; i < mNumActive; ++i) {
		int index = mActiveGibs[i];
		auto &gib = mGibs[index];
		gib.position += dt * gib.velocity;
		gib.velocity += gib.accel * dt;
		if (mCollider->collides(gib.position.x, gib.position.y)) {
			// Dampen bounce
			gib.velocity = -gib.velocity / 4.0f;
			if (glm::length2(gib.velocity) < 15 * 15) {
				// gib.accel = gib.velocity = Vec3(0.0f); // Stop
				gib.life = 0; // Kill
			}
		}

		gib.life -= dt;
		if (gib.life < 0) {
			addFree(index);
			// Swap
			mActiveGibs[i] = mActiveGibs[--mNumActive];
			// repeat this index
			--i;
		}
	}
}

void GibCollectionEntity::spawnGibs(const Vec3 &position, int amount) {
	const Vec3 GRAVITY_ACCEL(0, 1500.0f, 0);
	while (amount-- > 0) {
		int index = getFreeIndex();
		if (index < 0) return;

		addActive(index);
		Gib &gib = mGibs[index];
		gib.position = position;
		gib.size = Vec2(2 + getRandom() * 6);
		gib.life = gib.startLife = getRandom() * 1.0f + .5f;
		gib.velocity = Vec3(getSignedRandom() * 100, -getRandom() * 400 - 100, 0);
		gib.accel = GRAVITY_ACCEL;
	}
}