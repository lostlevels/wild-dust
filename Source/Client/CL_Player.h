#pragma once

#include "CL_Entity.h"
#include "CL_PhysicsEntity.h"
#include "CameraTarget.h"
#include "Shared/PlayerState.h"
#include "Shared/Teams.h"
#include "Shared/CharacterAnimationSet.h"

class AnimationSheet;
class Animation;
class AnimationRenderer;
class PlayerMovement;

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

	PlayerMovement *getPM() const { return mMovement; }

	Team getTeam() const { return mTeam; }

	bool mLookingLeft;

protected:
	CharacterAnimationSet mCowboyAnimSet;
	CharacterAnimationSet mBanditAnimSet;

	CharacterAnimationSet *getCurrentAnimSet();
	Animation *getCurrentAnim();

private:
	PlayerState mState;

	PlayerMovement *mMovement;

	Team mTeam;
};
