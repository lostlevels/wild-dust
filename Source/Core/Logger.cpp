#include "Precompiled.h"
#include "Logger.h"

Logger gLogger;

void Logger::init() {
#ifdef _WIN32
	mUsingStdio = false;
#else
	mUsingStdio = true;
#endif
}

void Logger::shutdown() {

}

void Logger::info(const char *format, ...) {
	va_list args;
	va_start(args, format);
#ifdef _WIN32
	if (mUsingStdio) {
		vfprintf(stdout, format, args);
	}
	else {
		char message[1024];
		vsprintf(message, format, args);
		OutputDebugStringA(message);
	}
#else
	vfprintf(stdout, format, args);
#endif
	va_end(args);
}

void Logger::error(const char *format, ...) {
	va_list args;
	va_start(args, format);
#ifdef _WIN32
	if (mUsingStdio) {
		vfprintf(stderr, format, args);
	}
	else {
		char message[1024];
		vsprintf(message, format, args);
		OutputDebugStringA(message);
	}
#else
	vfprintf(stderr, format, args);
#endif
	va_end(args);
}

