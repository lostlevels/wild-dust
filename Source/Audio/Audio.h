#pragma once

#include <al.h>
#include <alc.h>

class AudioSource;
class Music;
class SoundEffect;

class AUDIO_API AudioSystem {
public:
	AudioSystem();

	bool init();
	void shutdown();

	void update();

	bool hasAudioSupport() const { return mDevice && mContext; }

	Music *createMusic();
	void destroyMusic(Music *music);

	SoundEffect *createSoundEffect();
	void destroySoundEffect(SoundEffect *sfx);

private:
	ALCdevice *mDevice;
	ALCcontext *mContext;
	std::vector<AudioSource*> mSources;
};
