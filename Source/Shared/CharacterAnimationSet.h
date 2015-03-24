#pragma once

class AnimationSheet;
class Animation;
class AnimationRenderer;

struct CharacterAnimationSet {
	AnimationSheet *mAnimSheet;
	Animation *mIdleAnim;
	Animation *mWalkAnim;
	Animation *mJumpAnim;
	Animation *mShootAnim;
	AnimationRenderer *mAnimRenderer; // Must be freed MANUALLY
};

extern SHARED_API void freeCharacterAnimationSet(CharacterAnimationSet *set);
