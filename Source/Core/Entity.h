#pragma once

#include "Core/BitStream.h"
#include "Core/TransformSnapshot.h"
#include "Core/CommandSnapshot.h"
#include "Core/EntityRepresentation.h"
#include "Core/SpriteAnimator.h"

#include <string>
#include <vector>

enum {
	SEND_ALWAYS = 0,
	SEND_ENTER,
	SEND_NEVER
};

class Controller;

//
// Should probably use ECS next time since this class is getting bulky with lerp and animation
//

class CORE_API Entity {
public:
	Entity();
	Entity(const std::string &id, const std::string &type = "", const std::string &owner = "", int sendMode = SEND_ALWAYS);
	Entity(const Entity &other);
	Entity(const std::string &id, const std::string &type, const std::string &owner, int sendMode, const EntityRepresentation &representation);
	virtual ~Entity();

	virtual void update(float gameTime, float dt);

	const std::string &getId() const { return mId; }
	void setId(const std::string &id) { mId = id; }

	const std::string &getOwner() const { return mOwner; }
	void setOwner(const std::string &owner) { mOwner = owner; }

	const std::string &getType() const { return mType; }
	void setType(const std::string &type) { mType = type; }

	void setPosition(const Vec3 &position) { mPosition = position; }
	const Vec3 &getPosition() const { return mPosition; }
	// Prefer these to allow collision system to work
	void setDesiredPosition(const Vec3 &position) { mDesiredPosition = position; }
	const Vec3 &getDesiredPosition() const { return mDesiredPosition; }
	Vec3 getCenteredPosition() const;

	void setVelocity(const Vec3 &velocity) { mVelocity = velocity; }
	const Vec3 &getVelocity() const { return mVelocity; }
	void setAccel(const Vec3 &accel) { mAccel = accel; }
	const Vec3 &getAccel() const { return mAccel; }
	void interpolate(float time);

	const std::vector<TransformSnapshot> &getTransformSnapshots() const { return mTransformSnaps; }
	const std::vector<CommandSnapshot> &getCommandSnapshots() const { return mCommandSnaps; }
	void addSnapshots(const std::vector<TransformSnapshot> &snapshots);
	void queueTransformSnapshot(float time, uint32_t keys);

	float getLatestCommandTime() const;
	void getCommandSnapshotsAfter(float time, std::vector<CommandSnapshot> &outSnapshots) const;

	TransformSnapshot getSnapshot(float time);
	CommandSnapshot getClosestCommandSnapshot(float time);

	void addSnapshot(const CommandSnapshot &snapshot) { mCommandSnaps.push_back(snapshot); }
	void addSnapshot(const TransformSnapshot &snapshot) { mTransformSnaps.push_back(snapshot); }
	void removeOldSnapshots();
	void removeAllSnapshots();
	void removeAllTransformSnapshots();
	void removeAllCommandSnapshots();

	void setRepresentation(const EntityRepresentation &rep) { mRepresentation = rep; }
	const EntityRepresentation &getRepresentation() const { return mRepresentation; }

	void setSendMode(int mode) { mSendMode = mode; }
	int getSendMode() const { return mSendMode; }

	void setController(Controller *c);
	Controller *getController() const { return mController; }

	bool isLocal() const { return mLocal; }
	void setLocal(bool local) { mLocal = local; }
	bool isRemote() const { return !isLocal(); }

	void setFlip(int flip) { mFlip = flip; }
	int  getFlip() const { return mFlip; }

	// Just to not expose the animator.
	void animate(const std::string &animation, int loops);
	int  getFrame() const { return mAnimator.getFrame(); }
	void stopAnimating() { mAnimator.stopAnimating(); }
	bool isAnimating() const { return mAnimator.isAnimating(); }
	const std::string &getAnimation() const { return mAnimation; }

	void setViewDelay(float delay) { mViewDelay = delay; }
	float getViewDelay() const { return mViewDelay; }

	virtual std::string getRenderType() const { return "sprite"; }

private:
	Controller      *mController;
	SpriteAnimator  mAnimator;
	std::string     mAnimation;
	std::string     mId;
	std::string     mType;
	std::string     mOwner;

	float           mViewDelay;
	int             mSendMode;
	bool            mLocal;
	Vec3            mPosition;
	Vec3            mDesiredPosition;
	int             mFlip;
	Vec3            mVelocity;
	Vec3            mAccel;

	std::vector<TransformSnapshot> mTransformSnaps;
	std::vector<CommandSnapshot> mCommandSnaps;
	EntityRepresentation mRepresentation;
};
