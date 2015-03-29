#pragma once

#include "Entity.h"
#include <vector>

#define MAX_GIBS 256

//
// GibCollection so we dont have to make an entity for each gib / do any allocations
//

struct Gib {
	Vec2 size;
	Vec3 position;
	Vec3 velocity;
	Vec3 accel;
	float life;
	float startLife;
};

class WorldCollision;

class GibCollectionEntity : public Entity {
public:
	GibCollectionEntity(WorldCollision *collider, const std::string &id, const EntityRepresentation &rep);
	virtual ~GibCollectionEntity();

	virtual void update(float gameTime, float dt);

	virtual std::string getRenderType() const { return "gibs"; }
	const Gib *getGibs() const { return mGibs; }
	int getNumGibs() const { return mNumActive; }
	const int *getGibIndices() const { return mActiveGibs; }

	void spawnGibs(const Vec3 &position, int amount);

private:
	WorldCollision *mCollider;
	Gib mGibs[MAX_GIBS];
	int mFreeGibs[MAX_GIBS];
	int mNumFree;
	int mActiveGibs[MAX_GIBS];
	int mNumActive;

	int getFreeIndex(){ return mNumFree > 0 ? mFreeGibs[--mNumFree] : -1; }
	void addActive(int index) { if (mNumActive < MAX_GIBS) mActiveGibs[mNumActive++] = index; }
	void addFree(int index) { if (mNumFree < MAX_GIBS) mFreeGibs[mNumFree++] = index; }
};
