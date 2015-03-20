#include "Precompiled.h"
#include "Audio.h"
#include "Music.h"

AudioSystem::AudioSystem() {
	mDevice = NULL;
	mContext = NULL;
}

bool AudioSystem::init() {
	mDevice = alcOpenDevice(NULL);
	if (mDevice == NULL) {
		gLogger.error("Could not open audio device, no audio will be played.\n");
		return false;
	}

	mContext = alcCreateContext(mDevice, NULL);
	if (mContext == NULL) {
		gLogger.error("Could not create audio context, no audio will be played.\n");
		alcCloseDevice(mDevice);
		mDevice = NULL;
		return false;
	}

	alcMakeContextCurrent(mContext);

	gLogger.info("Successfuly opened audio device and created audio context.\n");

	mAnnoying = new Music();
	mAnnoying->openOggVorbisStream("../Content/Music/short.ogg");
	mAnnoying->play();
	mAnnoying->mLoops = -1;

	return true;
}

void AudioSystem::shutdown() {
	delete mAnnoying;

	if (mDevice) {
		if (mContext) {
			alcDestroyContext(mContext);
			mContext = NULL;
		}
		alcCloseDevice(mDevice);
		mDevice = NULL;
	}
}

void AudioSystem::update() {
	mAnnoying->update();
}
