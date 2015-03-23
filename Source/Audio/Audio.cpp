#include "Precompiled.h"
#include "Audio.h"
#include "Music.h"
#include "SoundEffect.h"

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

	return true;
}

void AudioSystem::shutdown() {
	for (AudioSource *source : mSources) {
		delete source;
	}
	mSources.clear();

	if (mDevice) {
		if (mContext) {
			alcDestroyContext(mContext);
			mContext = NULL;
		}
		alcCloseDevice(mDevice);
		mDevice = NULL;
	}
}

Music *AudioSystem::createMusic() {
	Music *music = new Music();
	mSources.push_back(music);
	return music;
}

void AudioSystem::destroyMusic(Music *music) {
	for (auto it = mSources.begin(); it != mSources.end(); ++it) {
		if (*it == music) {
			mSources.erase(it);
			break;
		}
	}
	delete music;
}

SoundEffect *AudioSystem::createSoundEffect() {
	SoundEffect *sfx = new SoundEffect();
	mSources.push_back(sfx);
	return sfx;
}

void AudioSystem::destroySoundEffect(SoundEffect *sfx) {
	for (auto it = mSources.begin(); it != mSources.end(); ++it) {
		if (*it == sfx) {
			mSources.erase(it);
			break;
		}
	}
	delete sfx;
}

void AudioSystem::update() {
	for (AudioSource *source : mSources) {
		source->update();
	}
}
