#include "Core/Precompiled.h"
#include "Core/Core.h"
#include "Core/Config.h"
#include "Core/Entity.h"

#include "Network/Precompiled.h"
#include "Network/Connection.h"

#include "Server/Precompiled.h"
#include "Server/ServerWorld.h"

#include "Game/Precompiled.h"
#include "Game/EntityFactory.h"

#include <enet/enet.h>

void UpdateConsoleTitle(ServerWorld *server) {
	char title[512];
	sprintf(title, "Dedicated Server (port %d) - [%d/%d] players - (tick: %d, send: %d)",
		server->getPort(),
		server->getNumPlayers(),
		server->getMaxPlayers(),
		server->getTickRate(),
		server->getSendRate());
#ifdef _WIN32
	SetConsoleTitleA(title);
#endif
}

#ifdef _WIN32
#ifdef main
#undef main
#endif
#endif

int main(int argc, char *argv[])
{
#ifdef _WIN32
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
#endif
#endif

	gCore.init();
	gLogger.useStdio();

	Config cfg;
	cfg.loadFromFile("Config/Dedicated.cfg");

	ServerWorld *serverWorld = new ServerWorld();
	serverWorld->serve(cfg.getInt("PortNumber", 5000));
	// TODO: the rest
//			cfg.getInt("TickRate", 64),
//			cfg.getInt("SendRate", 64),
//			cfg.getInt("MaxPlayers", 64)

	Clock titleUpdateTimer;
	Clock clock;
	while (true) {
		float dt = clock.getElapsedSeconds();
		clock.reset();
		serverWorld->update(dt);

		if (titleUpdateTimer.getElapsedSeconds() >= (1.0f / 25.0f)) {
			UpdateConsoleTitle(serverWorld);
			titleUpdateTimer.reset();
		}
	}

	delete serverWorld;

	gCore.shutdown();

	enet_deinitialize();

	return EXIT_SUCCESS;
}