#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>
#endif
#include "Client/Precompiled.h"
#include "Client/Client.h"
#include "Server/Precompiled.h"
#include "Server/Server.h"
#include <enet/enet.h>

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char *argv[])
#endif
{
	enet_initialize();

	Server *server = new Server();
	server->init();
	
	Client *client = new Client();
	client->init();
	
	while (!client->isQuitSignaled())
	{
		server->tick();
		client->tick();
	}

	server->shutdown();
	delete server;

	client->shutdown();
	delete client;

	enet_deinitialize();

	return 0;
}