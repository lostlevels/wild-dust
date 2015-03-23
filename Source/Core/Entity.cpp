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
	TransformSnapshot snapshot = getSnapshot(time);
	mPosition = snapshot.mPosition;
}

TransformSnapshot Entity::getSnapshot(float time) {
	if (mSnapshots.empty() || time < mSnapshots[0].mTime || time > mSnapshots[mSnapshots.size() - 1].mTime)
		return TransformSnapshot();
	
	for (int i = mSnapshots.size() - 2; i > -1; --i) {
		auto &snapshot = mSnapshots[i];
		if (snapshot.mTime < time) {
			auto &snapshotAfter = mSnapshots[i + 1];
			float t = (time - snapshot.mTime) / (snapshotAfter.mTime - snapshot.mTime);
			
			TransformSnapshot lerpedShot;
			lerpedShot.mPosition = glm::mix(snapshot.mPosition, snapshotAfter.mPosition, t);
			lerpedShot.mTime = time;
			
			return lerpedShot;
		}
	}
	
	return TransformSnapshot();
}