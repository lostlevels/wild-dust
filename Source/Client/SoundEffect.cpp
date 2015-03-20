#include "Precompiled.h"
#include "SoundEffect.h"

SoundEffect::SoundEffect() {
	alGenBuffers(1, &mBufferName);
	alSourcei(getALName(), AL_STREAMING, AL_FALSE);
}

SoundEffect::~SoundEffect() {
	alDeleteBuffers(1, &mBufferName);
}

bool SoundEffect::loadWave(const std::string &filename) {
	FILE *file = fopen(filename.c_str(), "rb");
	if (file == NULL) {
		gLogger.error("Could not open Wave file %s\n", filename.c_str());
		return false;
	}

	// Read master chunk header

	char chunkID[4];
	fread(chunkID, 1, 4, file);

	if (memcmp(chunkID, "RIFF", 4) != 0) {
		gLogger.error("File %s is not a RIFF file.\n", filename.c_str());
	}

	fseek(file, 4, SEEK_CUR);

	char waveID[4];
	fread(waveID, 1, 4, file);

	// Read fmt subchunk

	fread(chunkID, 1, 4, file);

	if (memcmp(chunkID, "fmt ", 4) != 0) {
		gLogger.error("Wave file %s contains invalid format chunk.\n", filename.c_str());
		fclose(file);
		return false;
	}

	fseek(file, 4, SEEK_CUR);
	
	int16_t format;
	fread(&format, 1, 2, file);

	if (format != 1) {
		gLogger.error("Wave file %s is not a PCM file.\n", filename.c_str());
		fclose(file);
		return false;
	}

	fread(&mNumChannels, 1, 2, file);
	fread(&mSampleRate, 1, 4, file);

	fseek(file, 6, SEEK_CUR);

	int16_t bitsPerSample;
	fread(&bitsPerSample, 1, 2, file);

	if (bitsPerSample != 16 && bitsPerSample != 8) {
		gLogger.error("Unsupported bit depth for wave file %s\n", filename.c_str());
		fclose(file);
		return false;
	}

	fread(chunkID, 1, 4, file);
	if (memcmp(chunkID, "data", 4) != 0) {
		gLogger.error("Encountered non-data chunk in Wave file %s\n", filename.c_str());
		fclose(file);
		return false;
	}

	int32_t dataChunkSize;
	fread(&dataChunkSize, 1, 4, file);

	uint8_t *samples = new uint8_t[dataChunkSize];
	fread(samples, 1, dataChunkSize, file);

	ALenum bufferFormat = AL_NONE;

	if (bitsPerSample == 8) {
		if (mNumChannels == 2) {
			bufferFormat = AL_FORMAT_STEREO8;
		}
		else {
			bufferFormat = AL_FORMAT_MONO8;
		}
	}
	else if (bitsPerSample == 16) {
		if (mNumChannels == 2) {
			bufferFormat = AL_FORMAT_STEREO16;
		}
		else {
			bufferFormat = AL_FORMAT_MONO16;
		}
	}

	alBufferData(
		mBufferName,
		bufferFormat,
		samples,
		dataChunkSize,
		mSampleRate);

	alSourcei(getALName(), AL_BUFFER, mBufferName);

	delete[] samples;

	fclose(file);

	return true;
}
