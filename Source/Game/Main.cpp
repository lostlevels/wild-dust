#include "Core/Precompiled.h"
#include "Core/Core.h"
#include "Core/Config.h"
#include <enet/enet.h>

#include "Core/EventEmitter.h"
#include "Network/Connection.h"

#include "Renderer/Precompiled.h"
#include "Renderer/SDLContext.h"
#include "Renderer/Renderer.h"

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
	gCore.init();

	Config settings;
	settings.loadFromFile("Config/Game.cfg");

	SDLContext context;
	context.init(settings);

	Renderer renderer(&context);
	renderer.init();

	Connection s;
	Connection c;
	s.serve(1336);
	c.connect("127.0.0.1", 1336);

	while (!context.isQuitSignaled()) {
		context.tick();

		s.processNetworkEvents();
		c.processNetworkEvents();
	}

	gCore.shutdown();

	enet_deinitialize();

	return 0;
}