#pragma once

#include <al.h>
#include <alc.h>

class Music;
class SoundEffect;

class AudioSystem {
public:
	AudioSystem();

	bool init();
	void shutdown();

	void update();

	bool hasAudioSupport() const { return mDevice && mContext; }

private:
	ALCdevice *mDevice;
	ALCcontext *mContext;
	Music *mAnnoying;
	SoundEffect *mTeleport;
};
