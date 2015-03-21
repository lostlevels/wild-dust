#include "Precompiled.h"
#include "Clock.h"

Clock::Clock() {
#ifdef _WIN32
	QueryPerformanceFrequency(&mCounterFrequency);
#endif
	reset();
}

void Clock::reset() {
#ifdef _WIN32
	QueryPerformanceCounter(&mLastSet);
#endif
}

float Clock::getElapsedSeconds() const {
#ifdef _WIN32
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);
	return float(now.QuadPart - mLastSet.QuadPart) / (float)mCounterFrequency.QuadPart;
#else
    return .01f;
#endif
}
