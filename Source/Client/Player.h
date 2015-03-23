#pragma once

#include "Entity.h"
#include "CameraTarget.h"
#include "Shared/PlayerState.h"

class AnimationSheet;
class Animation;

class CL_Player : public CL_Entity, public ICameraTarget {
public:
	CL_Player(Client *client);
	virtual ~CL_Player();

	virtual void readFromStream(const BitStream &stream);

	virtual void update(float dt);

	virtual void draw();

	virtual Vec2 ICameraTarget_getPosition() const;
	virtual Vec2 ICameraTarget_getSize() const;

	const Vec2 &getPosition() const { return mPosition; }
	const Vec2 &getSize() const { return mSize; }

	PlayerState getState() const { return mState; }

protected:
	AnimationSheet *mAnimSheet;
	Animation *mIdleAnimation;
	Animation *mWalkAnimation;
	Animation *mJumpAnimation;
	Animation *mShootAnimation;

private:
	Animation *getCurrentAnim();

private:
	Vec2 mPosition;
	Vec2 mSize;

	PlayerState mState;
};
