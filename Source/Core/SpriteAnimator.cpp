#include "Precompiled.h"
#include "SpriteAnimator.h"

SpriteAnimator::SpriteAnimator() :
	mLoops(0),
	mFrame(-1),
	mDuration(0),
	mElapsed(0)
{
}

void SpriteAnimator::animate(const SpriteAnimation &anim, int loops) {
	animate(anim.startFrame, anim.endFrame, anim.finishFrame, anim.duration, loops);
}

void SpriteAnimator::animate(int startFrame, int endFrame, int finishFrame, float duration, int loops) {
	if (loops == 0) loops = 1;
	duration = std::max(duration, .00001f);
	mStartFrame = startFrame;
	mEndFrame = endFrame;
	mFinishFrame = finishFrame;
	mLoops = loops;
	mDuration = duration;
	mElapsed = 0;
}

void SpriteAnimator::update(float dt) {
	if (isAnimating()) {
		mElapsed += dt;
		if (mElapsed > mDuration) {
			mElapsed -= mDuration;
			--mLoops;

			if (!isAnimating()) {
				mFrame = mFinishFrame;
				return;
			}
		}

		float t = mElapsed / mDuration;
		mFrame = (int)(mStartFrame + (mEndFrame - mStartFrame) * t);
	}
}