#pragma once

class CORE_API Logger {
public:
	// Used only by the core library, do not call from outside the core library.
	void init();
	void shutdown();

	void useStdio() { mUsingStdio = true; }

	void info(const char *format, ...);
	void error(const char *format, ...);

private:
	bool mUsingStdio;
};

extern CORE_API Logger gLogger;
