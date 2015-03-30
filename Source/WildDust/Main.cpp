#include "Core/Precompiled.h"
#include "Core/Core.h"
#include "Core/Config.h"
#include "Core/Entity.h"
#include "Core/Logger.h"

#include "Network/Precompiled.h"
#include "Network/Connection.h"

#include "Renderer/Precompiled.h"
#include "Renderer/SDLContext.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderGather.h"
#include "Renderer/GUI.h"

#include "Client/Precompiled.h"
#include "Client/ClientWorld.h"

#include "Server/Precompiled.h"
#include "Server/ServerWorld.h"

#include "Input/Precompiled.h"
#include "Input/Input.h"

#include "Audio/Audio.h"
#include "Audio/Music.h"

#include "Game/Precompiled.h"
#include "Game/EntityFactory.h"

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

#ifdef __APPLE__
#import <CoreFoundation/CoreFoundation.h>
#include <unistd.h>
#include <dirent.h>

static void initializeOSXPaths() {
	char path[PATH_MAX];
	CFURLRef res = CFBundleCopyExecutableURL(CFBundleGetMainBundle());
	CFURLGetFileSystemRepresentation(res, TRUE, (UInt8 *)path, PATH_MAX);
	CFRelease(res);
	
	std::string newpath = path;
	if (newpath.rfind("/") != std::string::npos) {
		int index = newpath.rfind("/");
		newpath = newpath.substr(0, index) + "";
	}
	
	if (chdir(newpath.c_str()) == -1) {
		gLogger.error("Failed to change path. Errno = %d\n", errno);
	}
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
	gCore.init();
	
#ifdef __APPLE__
	initializeOSXPaths();
#endif

	Config settings;
	settings.loadFromFile("Config/Game.cfg");

	SDLContext context;
	context.init(settings);

	RenderGather renderGather;
	Renderer renderer(&context);
	renderer.init();
	GUI gui(&renderer);
	gui.init();

	InputSystem input;

	AudioSystem audio;
	audio.init();
	Music *music = nullptr;

	if (settings.getBool("Music", true)) {
		music = audio.createMusic();
		music->openOggVorbisStream("../Content/Music/Texas.ogg");
		music->mLoops = -1;
		music->play();
		music->setVolume(0.15f);
	}

	ClientWorld *clientWorld = new ClientWorld(&input, &audio);
	ServerWorld *serverWorld = nullptr;

#ifdef _WIN32
	if (strstr(lpCmdLine, "-server")) {
#else
	if (ArgvContains(argv, argc, "-server")) {
#endif
		Config cfg;
		cfg.loadFromFile("Config/Listen.cfg");
		serverWorld = new ServerWorld();
		serverWorld->serve(cfg.getInt("PortNumber", 5000));
		clientWorld->connect("127.0.0.1", cfg.getInt("PortNumber", 5000));
	}
	else {
		//clientWorld->connect("37.142.124.227", settings.getInt("PortNumber", 5000));
		std::string ip = settings.getString("ServerIP", "127.0.0.1");
		gLogger.info("IP is %s\n", ip.c_str());
		clientWorld->connect(ip, settings.getInt("PortNumber", 5000));
	}

	Clock clock;
	while (!context.isQuitSignaled()) {
		context.tick();

		float dt = clock.getElapsedSeconds();
		clock.reset();

		clientWorld->update(dt);
		if (serverWorld)
			serverWorld->update(dt);

		if (music)
			music->update();

		// Update input last
		input.update(dt);

		std::vector<GUIData> data;
		clientWorld->fillGUIData(context.getWindowWidth(), context.getWindowHeight(), data);
		renderGather.drawWorld(clientWorld, &renderer, &gui, data);
	}

	clientWorld->disconnect();
	delete clientWorld;
	delete serverWorld;

	gui.shutdown();
	renderer.shutdown();

	if (music)
		audio.destroyMusic(music);

	audio.shutdown();
	gCore.shutdown();

	enet_deinitialize();

	return 0;
}