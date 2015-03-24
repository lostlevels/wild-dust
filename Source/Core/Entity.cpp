#include "Precompiled.h"
#include "Entity.h"

// Keep this many snapshots in case of dropped packets.
#define MIN_SNAPSHOT_HISTORY 240

Entity::Entity(const std::string &id, const std::string &type, const std::string &owner, int sendMode) :
	mId(id),
	mType(type),
	mOwner(owner),
	mSendMode(sendMode),
	mPosition(0.0f) {

	mSnapshots.reserve(128);
}

Entity::Entity() : Entity("", "", "", SEND_ALWAYS) {

}

Entity::Entity(const Entity &other) {
	if (&other != this) {
		*this = other;
	}
}

Entity::Entity(const std::string &id, const std::string &type, const std::string &owner,
	int sendMode, const EntityRepresentation &representation) :
	Entity(id, type, owner, sendMode) {
	setRepresentation(representation);
}

Entity::~Entity() {

}

void Entity::update(float dt) {

}

void Entity::addSnapshots(const std::vector<TransformSnapshot> &snapshots) {
	mSnapshots.insert(mSnapshots.end(), snapshots.begin(), snapshots.end());
}

void Entity::queueSnapshot(float time) {
	TransformSnapshot snapshot = {time, mPosition, 0};
	mSnapshots.push_back(snapshot);
}

void Entity::removeOldSnapshots() {
	if (mSnapshots.size() > MIN_SNAPSHOT_HISTORY) {
		mSnapshots.erase(mSnapshots.begin(), mSnapshots.begin() + mSnapshots.size() - MIN_SNAPSHOT_HISTORY);
	}
}

void Entity::interpolate(float time) {
	TransformSnapshot snapshot = getSnapshot(time);
	mPosition = snapshot.position;
}

TransformSnapshot Entity::getSnapshot(float time) {
	if (mSnapshots.empty() || time < mSnapshots[0].time || time > mSnapshots[mSnapshots.size() - 1].time)
		return TransformSnapshot();

	for (int i = mSnapshots.size() - 2; i > -1; --i) {
		auto &snapshot = mSnapshots[i];
		if (snapshot.time < time) {
			auto &snapshotAfter = mSnapshots[i + 1];
			float t = (time - snapshot.time) / (snapshotAfter.time - snapshot.time);

			TransformSnapshot lerpedShot;
			lerpedShot.position = glm::mix(snapshot.position, snapshotAfter.position, t);
			lerpedShot.time = time;

			return lerpedShot;
		}
	}

	return TransformSnapshot();
}