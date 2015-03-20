#include "Precompiled.h"
#include "Logger.h"

Logger gLogger;

void Logger::init() {

}

void Logger::shutdown() {

}

void Logger::info(const char *format, ...) {
	va_list args;
	va_start(args, format);
#ifdef _WIN32
	char message[1024];
	vsprintf(message, format, args);
	OutputDebugStringA(message);
#else
	// TODO: port this..
#endif
	va_end(args);
}

void Logger::error(const char *format, ...) {
	va_list args;
	va_start(args, format);
#ifdef _WIN32
	char message[1024];
	vsprintf(message, format, args);
	OutputDebugStringA(message);
#else
	// TODO: port this..
#endif
	va_end(args);
}

