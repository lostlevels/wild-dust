#pragma once

class BitStream {
public:
	BitStream(uint8_t *dataBuffer, int dataBufferSize);

	void writeI8(int8_t x);
	void writeU8(uint8_t x);
	void writeI16(int16_t x) { writeAny(x); }
	void writeU16(uint16_t x) { writeAny(x); }
	void writeI32(int32_t x) { writeAny(x); }
	void writeU32(uint32_t x) { writeAny(x); }
	void writeI64(int64_t x) { writeAny(x); }
	void writeU64(uint64_t x) { writeAny(x); }

	int8_t readI8() const;
	uint8_t readU8() const;
	int16_t readI16() const { return readAny<int16_t>(); }
	uint16_t readU16() const { return readAny<uint16_t>(); }
	int32_t readI32() const { return readAny<int32_t>(); }
	uint32_t readU32() const { return readAny<uint32_t>(); }
	int64_t readI64() const { return readAny<int64_t>(); }
	uint64_t readU64() const { return readAny<uint64_t>(); }

	template<class T> void writeAny(const T &x);
	template<class T> T readAny() const;

private:
	uint8_t *mDataBuffer;
	int mDataBufferSize;
	mutable int mDataBufferPosition;
};

inline BitStream::BitStream(uint8_t *dataBuffer, int dataBufferSize) {
	mDataBuffer = dataBuffer;
	mDataBufferSize = dataBufferSize;
	mDataBufferPosition = 0;
}

inline void BitStream::writeI8(int8_t x) {
	mDataBuffer[mDataBufferPosition++] = x;
}

inline void BitStream::writeU8(uint8_t x) {
	mDataBuffer[mDataBufferPosition++] = x;
}

inline int8_t BitStream::readI8() const {
	return mDataBuffer[mDataBufferPosition++];
}

inline uint8_t BitStream::readU8() const {
	return mDataBuffer[mDataBufferPosition++];
}

template<class T> inline void BitStream::writeAny(const T &x) {
	memcpy(mDataBuffer + mDataBufferPosition, &x, sizeof(x));
	mDataBufferPosition += sizeof(x);
}

template<class T> inline T BitStream::readAny() const {
	T x;
	memcpy(&x, mDataBuffer + mDataBufferPosition, sizeof(x));
	mDataBufferPosition += sizeof(x);
	return x;
}
