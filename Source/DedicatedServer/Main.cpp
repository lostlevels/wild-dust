#include "Core/Precompiled.h"
#include "Core/Core.h"
#include "Core/Config.h"
#include "Core/Clock.h"
#include "Server/Precompiled.h"
#include "Server/Server.h"
#include <enet/enet.h>

void UpdateConsoleTitle(Server *server) {
	char title[512];
	sprintf(title, "Dedicated Server (port %d) - [%d/%d] players - (tick: %d, send: %d)",
		server->getPortNumber(),
		server->getNumConnectedPlayers(),
		server->getMaxPlayers(),
		server->getTickRate(),
		server->getSendRate());
#ifdef _WIN32
	SetConsoleTitleA(title);
#endif
}

int main(int argc, char *argv[])
{
#ifdef _WIN32
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
#endif
#endif

	enet_initialize();
	
	gCore.init();
	gLogger.useStdio();

	Config cfg;
	cfg.loadFromFile("Config/Dedicated.cfg");
	
	Server *server = new Server();
	if (!server->init(
			cfg.getInt("PortNumber", 5000),
			cfg.getInt("TickRate", 64),
			cfg.getInt("SendRate", 64),
			cfg.getInt("MaxPlayers", 64)
			))
	{
		return EXIT_FAILURE;
	}

	Clock titleUpdateTimer;
	
	while (true) {
		server->update();

		if (titleUpdateTimer.getElapsedSeconds() >= (1.0f / 25.0f)) {
			UpdateConsoleTitle(server);
			titleUpdateTimer.reset();
		}
	}

	server->shutdown();
	delete server;

	gCore.shutdown();

	enet_deinitialize();

	return EXIT_SUCCESS;
}