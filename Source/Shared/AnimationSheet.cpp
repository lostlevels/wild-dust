#include "Precompiled.h"
#include "AnimationSheet.h"
#include <tinyxml2.h>

using namespace tinyxml2;

Animation::Animation(AnimationSheet *sheet) {
	mSheet = sheet;
	mCurrentFrameIndex = 0;
	mSpeed = 10.0f;
	mTimer = 0.0f;
	mLoopCount = -1;
	mLoopsLeft = -1;
	mFlipX = false;
	mFlipY = false;
}

void Animation::addFrame(int index, const Recti &aabb) {
	AnimationFrame frame;
	frame.index = index;
	frame.aabb = aabb;
	mFrames.push_back(frame);
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

AnimationSheet::AnimationSheet() {
	mFrameWidth = 0;
	mFrameHeight = 0;
}

AnimationSheet::~AnimationSheet() {
	unload();
}

bool AnimationSheet::loadFromFile(const std::string &filename) {
	unload();

	XMLDocument doc;
	if (doc.LoadFile(filename.c_str()) != XML_NO_ERROR) {
		gLogger.error("Could not load animation sheet from %s\n", filename.c_str());
		return false;
	}

	XMLElement *sheetElem = doc.FirstChildElement("AnimationSheet");

	mImageFilename = sheetElem->Attribute("Texture");
	mFrameWidth = sheetElem->IntAttribute("FrameWidth");
	mFrameHeight = sheetElem->IntAttribute("FrameHeight");
	
	XMLElement *animElem = sheetElem->FirstChildElement("Animation");
	while (animElem) {
		std::string animName = animElem->Attribute("Name");
		Animation *anim = createAnimation(animName);

		XMLElement *frameElem = animElem->FirstChildElement("Frame");
		while (frameElem) {
			int frameIndex = frameElem->IntAttribute("Index");

			std::string aabbText = frameElem->Attribute("AABB");
			int x, y, w, h;
			sscanf(aabbText.c_str(), "%i %i %i %i", &x, &y, &w, &h);

			anim->addFrame(frameIndex, { x, y, w, h });

			frameElem = frameElem->NextSiblingElement("Frame");
		}

		animElem = animElem->NextSiblingElement("Animation");
	}

	return true;
}

void AnimationSheet::unload() {
	for (auto animIt : mAnimations) {
		delete animIt.second;
	}
	mAnimations.clear();
}

Animation *AnimationSheet::createAnimation(const std::string &name) {
	auto findResult = mAnimations.find(name);
	if (findResult != mAnimations.end()) {
		mAnimations.erase(findResult);
	}

	Animation *anim = new Animation(this);
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
