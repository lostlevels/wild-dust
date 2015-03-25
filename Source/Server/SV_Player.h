#pragma once

#include "SV_PhysicsEntity.h"
#include "Core/Clock.h"
#include "Shared/PlayerState.h"
#include "Shared/Teams.h"
#include "Shared/CharacterAnimationSet.h"
#include "Shared/PlayerInput.h"

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

	void processInput(PlayerInput input);
	void applyInput(PlayerInput input);

	bool mLookingLeft;

	PlayerMovement *getPM() const { return mMovement; }

	Team getTeam() const { return mTeam; }
	void changeTeam(Team newTeam) { mTeam = newTeam; }
	
	void inflictDamage(int amount);

	int getHealth() const { return mHealth; }

private:
	CharacterAnimationSet mCowboyAnimSet;
	CharacterAnimationSet mBanditAnimSet;

	CharacterAnimationSet *getCurrentAnimSet();
	Animation *getCurrentAnim();

protected:
	PlayerState mState;
	Clock mShootTimer;
	PlayerMovement *mMovement;
	Team mTeam;
	int mHealth;
	uint32_t mLastProcessedInputSequenceID;
};
