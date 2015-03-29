#pragma once

#include "AudioSource.h"

#define MUS_BUF_COUNT 3
#define MUS_DECODE_BUF_SIZE (44100)

typedef struct stb_vorbis stb_vorbis;

class AUDIO_API Music : public AudioSource {
public:
	Music();
	~Music();

	bool openOggVorbisStream(const std::string &filename);
	void closeStream();
	void rewindStream();

	void update();

	int mLoops;

private:
	int decodeSamples(bool *endOfStream);

private:
	stb_vorbis *mVorbisStream;
	short *mDecodeBuffer;
	ALuint mBuffers[MUS_BUF_COUNT];
	int mNumChannels;
	int mSampleRate;
	bool mIsEndOfStream;
};