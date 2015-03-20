#pragma once

#include "AudioSource.h"

class SoundEffect : public AudioSource {
public:
	SoundEffect();
	~SoundEffect();

	bool loadWave(const std::string &filename);

private:
	ALuint mBufferName;
	uint16_t mNumChannels;
	uint32_t mSampleRate;
};