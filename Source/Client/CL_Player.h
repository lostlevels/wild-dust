#pragma once

#include "CL_Entity.h"
#include "CL_PhysicsEntity.h"
#include "CameraTarget.h"
#include "Shared/PlayerState.h"

class AnimationSheet;
class Animation;

class CL_Player : public CL_PhysicsEntity, public ICameraTarget {
public:
	CL_Player(Client *client);
	virtual ~CL_Player();

	virtual void readFromStream(const BitStream &stream);

	virtual void update(float dt);

	virtual void draw();

	virtual Vec2 ICameraTarget_getPosition() const;
	virtual Vec2 ICameraTarget_getSize() const;

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
	PlayerState mState;
	bool mLookingLeft;
};
