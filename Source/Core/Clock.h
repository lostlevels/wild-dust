#pragma once

class CORE_API Clock {
public:
	Clock();

	void reset();
	float getElapsedSeconds() const;

private:
#ifdef _WIN32
	LARGE_INTEGER mCounterFrequency;
	LARGE_INTEGER mLastSet;
#else
    long mLastTime;
#endif
};
