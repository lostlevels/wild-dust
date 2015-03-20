#include "Precompiled.h"
#include "Core.h"
#include "Logger.h"

Core gCore;

void Core::init() {
	gLogger.init();
}

void Core::shutdown() {
	gLogger.shutdown();
}
