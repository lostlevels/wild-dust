#include "Precompiled.h"
#include "Clock.h"

#ifndef _WIN32
#include <sys/time.h>
static long getTimeOfDayMillis() {
    timeval time;
    gettimeofday(&time, NULL);
    return (time.tv_sec * 1000) + (time.tv_usec / 1000);
}
#endif

Clock::Clock() {
#ifdef _WIN32
	QueryPerformanceFrequency(&mCounterFrequency);
#endif
	reset();
}

void Clock::reset() {
#ifdef _WIN32
	QueryPerformanceCounter(&mLastSet);
#else
    mLastTime = getTimeOfDayMillis();
#endif
}

float Clock::getElapsedSeconds() const {
#ifdef _WIN32
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);
	return float(now.QuadPart - mLastSet.QuadPart) / (float)mCounterFrequency.QuadPart;
#else
    return (getTimeOfDayMillis() - mLastTime) / 1000.0f;
#endif
}
