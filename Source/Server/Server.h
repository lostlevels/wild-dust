#pragma once

#include <enet/enet.h>

class SERVER_API Server {
public:
	bool init();
	void shutdown();
	void tick();
};
