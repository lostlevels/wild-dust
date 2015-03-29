#pragma once

struct SpriteAnimation {
	int startFrame;
	int endFrame;
	int finishFrame; // Frame to go on when animation is done - usually but not always is the endFrame
	float duration;
};
