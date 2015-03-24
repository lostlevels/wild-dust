#pragma once

#include "SV_PhysicsEntity.h"
#include "Core/Clock.h"
#include "Shared/PlayerState.h"

class PlayerMovement;
class AnimationSheet;
class Animation;

class SV_Player : public SV_PhysicsEntity {
public:
	SV_Player(Server *server);
	virtual ~SV_Player();

	virtual void update(float dt);

	virtual void writeToStream(BitStream &stream);

	virtual void shoot();

	bool mLookingLeft;

	PlayerMovement *getPM() const { return mMovement; }

private:
	Animation *getCurrentAnim();

protected:
	PlayerState mState;
	Clock mShootTimer;
	PlayerMovement *mMovement;
	AnimationSheet *mAnimSheet;
	Animation *mIdleAnimation;
	Animation *mWalkAnimation;
	Animation *mJumpAnimation;
	Animation *mShootAnimation;
};
