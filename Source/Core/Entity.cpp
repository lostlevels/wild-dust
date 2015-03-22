#include "Precompiled.h"
#include "Entity.h"

// Keep this many snapshots in case of dropped packets.
#define MIN_SNAPSHOT_HISTORY 240

Entity::Entity(const std::string &owner) : mOwner(owner),
	mPosition(0.0f) {

}

Entity::~Entity() {

}

void Entity::update(float dt) {

}

void Entity::addSnapshots(const std::vector<TransformSnapshot> &snapshots) {
	mSnapshots.insert(mSnapshots.end(), snapshots.begin(), snapshots.end());
}

void Entity::removeOldSnapshots() {
	if (mSnapshots.size() > MIN_SNAPSHOT_HISTORY) {
		mSnapshots.erase(mSnapshots.begin(), mSnapshots.begin() + mSnapshots.size() - MIN_SNAPSHOT_HISTORY);
	}
}

void Entity::interpolate(float time) {

}

TransformSnapshot Entity::getSnapshot(float time) {

}