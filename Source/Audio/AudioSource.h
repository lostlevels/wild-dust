#pragma once

#include <al.h>

class AUDIO_API AudioSource {
public:
	AudioSource();
	virtual ~AudioSource();

	virtual void update() {}

	virtual void play();
	virtual void stop();
	virtual bool isPlaying() const;

	virtual float getVolume() const;
	virtual void setVolume(float vol);

	virtual float getPitch() const;
	virtual void setPitch(float pitch);

	virtual Vec2 getPosition() const;
	virtual void setPosition(const Vec2 &pos);

	virtual bool isRelativeToListener() const;
	virtual void setRelativeToListener(bool rel);

	ALuint getALName() const { return mALName; }

private:
	ALuint mALName;
};
