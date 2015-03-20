#include "Precompiled.h"
#include "Clock.h"

Clock::Clock() {
	QueryPerformanceFrequency(&mCounterFrequency);
	reset();
}

void Clock::reset() {
	QueryPerformanceCounter(&mLastSet);
}

float Clock::getElapsedSeconds() const {
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);
	return float(now.QuadPart - mLastSet.QuadPart) / (float)mCounterFrequency.QuadPart;
}
