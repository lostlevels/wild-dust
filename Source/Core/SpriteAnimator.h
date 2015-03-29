#pragma once

#include <algorithm>
#include "SpriteAnimation.h"

class CORE_API SpriteAnimator {
public:
	SpriteAnimator();

	void animate(int startFrame, int endFrame, int finishFrame, float duration, int loops);
	void animate(const SpriteAnimation &anim, int loops);
	void update(float dt);

	bool isAnimating() const { return mLoops != 0; }
	int  getFrame() const { return mFrame; }
	void stopAnimating() { mLoops = 0; }

private:
	int mStartFrame;
	int mEndFrame;
	int mFinishFrame;
	int mLoops;
	int mFrame;
	float mDuration;
	float mElapsed;
};
