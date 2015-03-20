#ifdef _WIN32
#include <Windows.h>
#endif
#include <SDL.h>

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char *argv[])
#endif
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window *gameWindow = SDL_CreateWindow(
		"Code a Game 2015",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		1280,
		720,
		SDL_WINDOW_OPENGL);

	SDL_GLContext context = SDL_GL_CreateContext(gameWindow);
	SDL_GL_MakeCurrent(gameWindow, context);

	while (true) {
		bool shouldQuit = false;

		SDL_Event evt;
		while (SDL_PollEvent(&evt)) {
			if (evt.type == SDL_QUIT) {
				shouldQuit = true;
				break;
			}
		}

		if (shouldQuit)
			break;

		SDL_GL_SwapWindow(gameWindow);
	}

	SDL_Quit();

	return 0;
}