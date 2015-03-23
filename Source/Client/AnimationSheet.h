#pragma once

class Renderer;
class AnimationSheet;
class Texture;

class Animation {
public:
	Animation(AnimationSheet *sheet, const std::vector<int> &frames);

	void animate(float dt);
	void draw(const Vec2 &position, const Vec2 &size, const Color &tint);

	// Reset to initial frame
	void reset() {
		mCurrentFrameIndex = 0;
		mTimer = 0.0f;
	}

	// Playback speed in frames per second
	float getSpeed() const { return mSpeed; }
	void setSpeed(float speed) { mSpeed = speed; }

private:
	AnimationSheet *mSheet;
	std::vector<int> mFrames;
	int mCurrentFrameIndex;
	float mSpeed;
	float mTimer;
};

class AnimationSheet {
public:
	AnimationSheet(Renderer *renderer);
	~AnimationSheet();

	bool loadFromFile(const std::string &filename, int frameWidth, int frameHeight);
	void unload();

	Animation *createAnimation(const std::string &name, const std::vector<int> &frames);
	Animation *findAnimation(const std::string &name);

	Renderer *getRenderer() { return mRenderer; }

	Texture *getTexture() const { return mTexture; }
	int getFrameWidth() const { return mFrameWidth; }
	int getFrameHeight() const { return mFrameHeight; }

private:
	Renderer *mRenderer;
	Texture *mTexture;
	int mFrameWidth;
	int mFrameHeight;
	std::map<std::string, Animation*> mAnimations;
};
