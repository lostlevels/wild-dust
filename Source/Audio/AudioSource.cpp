#include "Precompiled.h"
#include "AudioSource.h"
#include <al.h>

AudioSource::AudioSource() {
	alGenSources(1, &mALName);
	setRelativeToListener(true);
}

AudioSource::~AudioSource() {
	alDeleteSources(1, &mALName);
}

void AudioSource::play() {
	alSourcePlay(mALName);
}

void AudioSource::stop() {
	alSourceStop(mALName);
}

bool AudioSource::isPlaying() const{
	ALint state;
	alGetSourcei(mALName, AL_SOURCE_STATE, &state);
	return state == AL_PLAYING;
}

float AudioSource::getVolume() const {
	float vol;
	alGetSourcef(mALName, AL_GAIN, &vol);
	return vol;
}

void AudioSource::setVolume(float vol) {
	alSourcef(mALName, AL_GAIN, vol);
}

float AudioSource::getPitch() const {
	float pitch;
	alGetSourcef(mALName, AL_PITCH, &pitch);
	return pitch;
}

void AudioSource::setPitch(float pitch) {
	alSourcef(mALName, AL_PITCH, pitch);
}

Vec2 AudioSource::getPosition() const {
	Vec3 pos;
	alGetSource3f(mALName, AL_POSITION, &pos.x, &pos.y, &pos.z);
	return Vec2(pos.x, pos.y);
}

void AudioSource::setPosition(const Vec2 &pos) {
	alSource3f(mALName, AL_POSITION, pos.x, pos.y, 0.0f);
}

bool AudioSource::isRelativeToListener() const {
	ALint rel;
	alGetSourcei(mALName, AL_SOURCE_RELATIVE, &rel);
	return rel ? true : false;
}

void AudioSource::setRelativeToListener(bool rel) {
	alSourcei(mALName, AL_SOURCE_RELATIVE, rel ? AL_TRUE : AL_FALSE);
}

