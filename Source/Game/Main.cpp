#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>
#endif
#include "Core/Precompiled.h"
#include "Core/Core.h"
#include "Core/Config.h"
#include "Client/Precompiled.h"
#include "Client/Client.h"
#include "Server/Precompiled.h"
#include "Server/Server.h"
#include <enet/enet.h>

#ifndef _WIN32
bool ArgvContains(char *argv[], int argc, const char *what) {
	for (int i = 1; i < argc; ++i) {
		if (strcmp(argv[i], what) == 0) {
			return true;
		}
	}
	return false;
}
#endif

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char *argv[])
#endif
{
#ifdef _WIN32
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
#endif

	Server *listenServer = NULL;

	enet_initialize();

	gCore.init();

#ifdef _WIN32
	if (strstr(lpCmdLine, "-server")) {
#else
	if (ArgvContains(argv, argc, "-server")) {
#endif
		listenServer = new Server();
		listenServer->init(100, 100);
	}
	
	Client *client = new Client();
	client->init();
	client->connectToServer("127.0.0.1", 5000);
	
	while (!client->isQuitSignaled())
	{
		if (listenServer) {
			listenServer->update();
		}
		client->tick();
	}

	if (listenServer) {
		listenServer->shutdown();
		delete listenServer;
	}
	
	client->shutdown();
	delete client;

	gCore.shutdown();

	enet_deinitialize();

	return 0;
}