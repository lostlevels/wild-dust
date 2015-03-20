#include "Precompiled.h"
#include "Renderer.h"
#include "Client.h"

Renderer::Renderer(Client *context) {
	mContext = context;
}

bool Renderer::init() {
	mGLContext = SDL_GL_CreateContext(mContext->getGameWindow());
	SDL_GL_MakeCurrent(mContext->getGameWindow(), mGLContext);

	if (glewInit() != GLEW_OK)
		return false;

	return true;
}

void Renderer::shutdown() {
}

void Renderer::beginFrame() {
	
}

void Renderer::endFrame() {
	SDL_GL_SwapWindow(mContext->getGameWindow());
}
