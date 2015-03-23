#include "Precompiled.h"
#include "Music.h"
#undef STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>

Music::Music() {
	alGenBuffers(MUS_BUF_COUNT, mBuffers);
	mVorbisStream = NULL;
	mDecodeBuffer = reinterpret_cast<short*>(new uint8_t[MUS_DECODE_BUF_SIZE]);
	mNumChannels = 0;
	mSampleRate = 0;
	mIsEndOfStream = true;
	alSourcei(getALName(), AL_STREAMING, AL_TRUE);
	mLoops = 0;
}

Music::~Music() {
	closeStream();
	alDeleteBuffers(MUS_BUF_COUNT, mBuffers);
	delete[] reinterpret_cast<uint8_t*>(mDecodeBuffer);
}

bool Music::openOggVorbisStream(const std::string &filename) {
	mVorbisStream = stb_vorbis_open_filename(filename.c_str(), NULL, NULL);
	if (mVorbisStream == NULL) {
		gLogger.error("Could not open Ogg Vorbis stream from file %s\n", filename.c_str());
		return false;
	}

	stb_vorbis_info streamInfo = stb_vorbis_get_info(mVorbisStream);
	mNumChannels = streamInfo.channels;
	mSampleRate = streamInfo.sample_rate;

	mIsEndOfStream = false;
	mLoops = 0;

	for (int i = 0; i < MUS_BUF_COUNT; ++i) {
		decode:
		bool endOfStream = false;
		int bytesDecoded = decodeSamples(&endOfStream);
		if (bytesDecoded == 0) {
			break;
		}

		alBufferData(
			mBuffers[i],
			mNumChannels == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16,
			mDecodeBuffer,
			bytesDecoded,
			mSampleRate);

		alSourceQueueBuffers(getALName(), 1, &mBuffers[i]);

		if (endOfStream) {
			mIsEndOfStream = true;
			if (mLoops != 0) {
				--mLoops;
				rewindStream();
				goto decode;
			}
			else {
				closeStream();
			}
			break;
		}
	}

	return true;
}

void Music::closeStream() {
	if (mVorbisStream) {
		stb_vorbis_close(mVorbisStream);
		mVorbisStream = NULL;
	}
}

void Music::rewindStream() {
	if (mVorbisStream) {
		stb_vorbis_seek_start(mVorbisStream);
	}
}

void Music::update() {
	if (mVorbisStream == NULL) {
		return;
	}

	ALint processed;
	alGetSourcei(getALName(), AL_BUFFERS_PROCESSED, &processed);

	for (int i = 0; i < processed; ++i) {
		ALuint bufferName;
		alSourceUnqueueBuffers(getALName(), 1, &bufferName);

		decode:
		bool endOfStream = false;
		int bytesDecoded = decodeSamples(&endOfStream);

		if (bytesDecoded > 0) {
			alBufferData(
				bufferName,
				mNumChannels == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16,
				mDecodeBuffer,
				bytesDecoded,
				mSampleRate);

			alSourceQueueBuffers(getALName(), 1, &bufferName);
		}
		else {
			if (mLoops != 0) {
				--mLoops;
				rewindStream();
				goto decode;
			}
			else {
				closeStream();
			}
		}
	}
}

int Music::decodeSamples(bool *endOfStream) {
	int bytesDecoded = 0;

	while (bytesDecoded < MUS_DECODE_BUF_SIZE) {
		int decoded = stb_vorbis_get_samples_short_interleaved(
			mVorbisStream,
			mNumChannels,
			mDecodeBuffer + bytesDecoded / 2,
			(MUS_DECODE_BUF_SIZE - bytesDecoded) / 2);

		if (decoded == 0) {
			// End-of-stream reached
			if (endOfStream) {
				*endOfStream = true;
				return bytesDecoded;
			}
		}

		bytesDecoded += decoded * mNumChannels * 2;
	}

	if (endOfStream) {
		*endOfStream = false;
	}

	return bytesDecoded;
}
