#pragma once

class AnimationSheet;
class Texture;

struct AnimationFrame {
	int index;
	Recti aabb;
};

class SHARED_API Animation {
public:
	Animation(AnimationSheet *sheet);

	void addFrame(int index, const Recti &aabb);

	void animate(float dt);

	// Reset to initial frame
	void reset() {
		mCurrentFrameIndex = 0;
		mTimer = 0.0f;
		mLoopsLeft = mLoopCount;
	}

	const AnimationFrame &getCurrentFrame() const {
		if (mCurrentFrameIndex < mFrames.size()) {
			return mFrames[mCurrentFrameIndex];
		}
		else {
			return mFrames[0];
		}
	}

	int getCurrentFrameIndex() const { return mCurrentFrameIndex; }
	void setCurrentFrameIndex(int index) { mCurrentFrameIndex = index; }

	// Playback speed in frames per second (default: 10)
	float getSpeed() const { return mSpeed; }
	void setSpeed(float speed) { mSpeed = speed; }

	// Number of loops (-1 for infinite, default: -1)
	int getLoopCount() const { return mLoopCount; }
	void setLoopCount(int count) {
		mLoopCount = count;
		mLoopsLeft = count;
	}

	bool getFlipX() const { return mFlipX; }
	void setFlipX(bool flip) { mFlipX = flip; }

	bool getFlipY() const { return mFlipY; }
	void setFlipY(bool flip) { mFlipY = flip; }

private:
	AnimationSheet *mSheet;
	std::vector<AnimationFrame> mFrames;
	int mCurrentFrameIndex;
	float mSpeed;
	float mTimer;
	int mLoopCount;
	int mLoopsLeft;
	bool mFlipX;
	bool mFlipY;
};

class SHARED_API AnimationSheet {
public:
	AnimationSheet();
	~AnimationSheet();

	bool loadFromFile(const std::string &filename);
	void unload();

	Animation *createAnimation(const std::string &name);
	Animation *findAnimation(const std::string &name);

	std::string getImageFilename() const { return mImageFilename; }
	int getFrameWidth() const { return mFrameWidth; }
	int getFrameHeight() const { return mFrameHeight; }

private:
	std::string mImageFilename;
	int mFrameWidth;
	int mFrameHeight;
	std::map<std::string, Animation*> mAnimations;
};
