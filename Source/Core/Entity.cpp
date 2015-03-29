#include "Precompiled.h"
#include "Entity.h"
#include "Controller.h"

// Keep this many snapshots in case of dropped packets.
#define MIN_SNAPSHOT_HISTORY 240

template<typename T>
static void trimSnapshots(std::vector<T> &snapshots) {
	if (snapshots.size() > MIN_SNAPSHOT_HISTORY)
		snapshots.erase(snapshots.begin(), snapshots.begin() + snapshots.size() - MIN_SNAPSHOT_HISTORY);
}

Entity::Entity(const std::string &id, const std::string &type, const std::string &owner, int sendMode) :
	mController(nullptr),
	mId(id),
	mType(type),
	mOwner(owner),
	mViewDelay(0),
	mSendMode(sendMode),
	mLocal(true),
	mPosition(0.0f),
	mFlip(0),
	mVelocity(0.0f),
	mAccel(0.0f) {

	mTransformSnaps.reserve(128);
}

Entity::Entity() : Entity("", "", "", SEND_ALWAYS) {

}

Entity::Entity(const Entity &other) {
	if (&other != this)
		*this = other;
}

Entity::Entity(const std::string &id, const std::string &type, const std::string &owner,
	int sendMode, const EntityRepresentation &representation) :
	Entity(id, type, owner, sendMode) {
	setRepresentation(representation);
}

Entity::~Entity() {
	delete mController;
	mController = nullptr;
}

void Entity::update(float gameTime, float dt) {
	if (mController) {
		mController->control(this, gameTime, dt);
	}

	mAnimator.update(dt);
}

void Entity::animate(const std::string &animation, int loops) {
	const auto &anim = mRepresentation.animations.find(animation);
	if (anim != mRepresentation.animations.end()) {
		mAnimation = animation;
		mAnimator.animate(anim->second, loops);
	}
}

Vec3 Entity::getCenteredPosition() const {
	Vec3 position = getPosition();
	auto &rep = getRepresentation();
	position.x += rep.size.x / 2;
	position.y += rep.size.y / 2;

	return position;
}

void Entity::setController(Controller *c) {
	mController = c;
	if (mController)
		mController->onAttached(this);
}

void Entity::addSnapshots(const std::vector<TransformSnapshot> &snapshots) {
	mTransformSnaps.insert(mTransformSnaps.end(), snapshots.begin(), snapshots.end());
}

void Entity::queueTransformSnapshot(float time, uint32_t keys) {
	TransformSnapshot snapshot = {time, mPosition, 0, keys};
	mTransformSnaps.push_back(snapshot);
}

void Entity::removeOldSnapshots() {
	trimSnapshots<TransformSnapshot>(mTransformSnaps);
	trimSnapshots<CommandSnapshot>(mCommandSnaps);
}

void Entity::interpolate(float time) {
	TransformSnapshot snapshot = getSnapshot(time);
	mPosition = snapshot.position;
}

void Entity::removeAllTransformSnapshots() {
	mTransformSnaps.clear();
}

void Entity::removeAllCommandSnapshots() {
	mCommandSnaps.clear();
}
void Entity::removeAllSnapshots() {
	removeAllCommandSnapshots();
	removeAllTransformSnapshots();
}

// Interpolating across keys makes no sense so find closest instead
CommandSnapshot Entity::getClosestCommandSnapshot(float time) {
	if (mCommandSnaps.empty()) return CommandSnapshot();
	if (time < mCommandSnaps[0].time || mCommandSnaps.size() == 1) return mCommandSnaps[0];
	if (time > mCommandSnaps.back().time) return mCommandSnaps.back();

	for (int i = 0; i < (int)mCommandSnaps.size() - 1; ++i) {
		auto &snapBefore = mCommandSnaps[i];
		auto &snapAfter = mCommandSnaps[i + 1];
		if (time > snapBefore.time && time < snapAfter.time) {
			return fabsf(time - snapBefore.time) < fabsf(time - snapAfter.time) ? snapBefore : snapAfter;
		}
	}

	return mCommandSnaps[0];
}

float Entity::getLatestCommandTime() const {
	if (mCommandSnaps.empty()) return 0;
	return mCommandSnaps.back().time;
}

void Entity::getCommandSnapshotsAfter(float time, std::vector<CommandSnapshot> &outSnapshots) const {
	for (auto &snapshot : mCommandSnaps) {
		if (snapshot.time > time)
			outSnapshots.push_back(snapshot);
	}
}

TransformSnapshot Entity::getSnapshot(float time) {
	if (mTransformSnaps.empty()) return {0, Vec3(-100, -100, 0)}; // negative so offscreen
	if (time < mTransformSnaps[0].time || time > mTransformSnaps.back().time)
		return mTransformSnaps.back();

	for (int i = (int)mTransformSnaps.size() - 2; i > -1; --i) {
		auto &snapshot = mTransformSnaps[i];
		if (snapshot.time < time) {
			auto &snapshotAfter = mTransformSnaps[i + 1];
			float t = (time - snapshot.time) / (snapshotAfter.time - snapshot.time);

			TransformSnapshot lerpedShot;
			lerpedShot.position = glm::mix(snapshot.position, snapshotAfter.position, t);
			lerpedShot.time = time;
			lerpedShot.keys = snapshot.keys | snapshotAfter.keys;
			return lerpedShot;
		}
	}

	return TransformSnapshot();
}
