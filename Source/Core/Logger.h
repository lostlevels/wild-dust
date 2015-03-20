#pragma once

class CORE_API Logger {
public:
	// Used only by the core library, do not call from outside the core library.
	void init();
	void shutdown();

	void info(const char *format, ...);
	void error(const char *format, ...);
};

extern CORE_API Logger gLogger;
