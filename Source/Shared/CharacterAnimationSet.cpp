#include "Precompiled.h"
#include "CharacterAnimationSet.h"
#include "AnimationSheet.h"

void freeCharacterAnimationSet(CharacterAnimationSet *set) {
	delete set->mAnimSheet;
}
