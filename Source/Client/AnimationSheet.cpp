#include "Precompiled.h"
#include "AnimationSheet.h"
#include "Renderer.h"
#include "Texture.h"
#include "SpriteBatcher.h"

Animation::Animation(AnimationSheet *sheet, const std::vector<int>& frames) {
	mSheet = sheet;
	mFrames = frames;
	mCurrentFrameIndex = 0;
	mSpeed = 10.0f;
	mTimer = 0.0f;
	mLoopCount = -1;
	mLoopsLeft = -1;
}

void Animation::animate(float dt) {
	if (mLoopsLeft == 0) {
		return;
	}

	mTimer += dt;
	if (mTimer >= (1.0f / mSpeed)) {
		if (++mCurrentFrameIndex >= (int)mFrames.size()) {
			mCurrentFrameIndex = 0;
			--mLoopsLeft;
		}
		mTimer = 0.0f;
	}
}

void Animation::draw(const Vec2 &position, const Vec2 &size, const Color &tint) {
	Renderer *renderer = mSheet->getRenderer();

	int framesPerRow = mSheet->getTexture()->getWidth() / mSheet->getFrameWidth();

	int currentFrame = mFrames[mCurrentFrameIndex];
	int currentFrameX = currentFrame % framesPerRow;
	int currentFrameY = currentFrame / framesPerRow;

	Recti source;
	source.x = currentFrameX * mSheet->getFrameWidth();
	source.y = currentFrameY * mSheet->getFrameHeight();
	source.w = mSheet->getFrameWidth();
	source.h = mSheet->getFrameHeight();

	SpriteBatcher *batcher = renderer->getSpriteBatcher(mSheet->getTexture(), BLEND_ALPHA);
	batcher->addSprite(position, size, source, tint);
}

AnimationSheet::AnimationSheet(Renderer *renderer) {
	mRenderer = renderer;
	mTexture = NULL;
	mFrameWidth = 0;
	mFrameHeight = 0;
}

AnimationSheet::~AnimationSheet() {
	unload();
}

bool AnimationSheet::loadFromFile(const std::string &filename, int frameWidth, int frameHeight) {
	unload();

	mTexture = mRenderer->getTexture(filename);
	if (!mTexture) {
		return false;
	}

	mFrameWidth = frameWidth;
	mFrameHeight = frameHeight;

	return true;
}

void AnimationSheet::unload() {
	for (auto animIt : mAnimations) {
		delete animIt.second;
	}
	mAnimations.clear();

	if (mTexture) {
		mTexture->decrementRefs();
		mTexture = NULL;
	}
}

Animation *AnimationSheet::createAnimation(const std::string &name, const std::vector<int> &frames){
	auto findResult = mAnimations.find(name);
	if (findResult != mAnimations.end()) {
		mAnimations.erase(findResult);
	}

	Animation *anim = new Animation(this, frames);
	mAnimations.insert({ name, anim });

	return anim;
}

Animation *AnimationSheet::findAnimation(const std::string &name) {
	auto findResult = mAnimations.find(name);
	if (findResult != mAnimations.end()) {
		return findResult->second;
	}
	else {
		return NULL;
	}
}
