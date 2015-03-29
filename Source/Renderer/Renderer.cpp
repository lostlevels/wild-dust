#include "Precompiled.h"
#include "Renderer.h"
#include "Texture.h"
#include "Shader.h"
#include "SpriteBatcher.h"
#include "Font.h"
#include <SDL_ttf.h>
#include "GUI.h"

static void PumpOpenGLErrors(bool report = true) {
	while (GLenum error = glGetError()) {
		if (report) {
			switch (error) {
			case GL_INVALID_ENUM:
				gLogger.error("OpenGL error: GL_INVALID_ENUM\n");
				break;

			case GL_INVALID_VALUE:
				gLogger.error("OpenGL error: GL_INVALID_VALUE\n");
				break;

			case GL_INVALID_OPERATION:
				gLogger.error("OpenGL error: GL_INVALID_OPERATION\n");
				break;

			case GL_INVALID_FRAMEBUFFER_OPERATION:
				gLogger.error("OpenGL error: GL_INVALID_FRAMEBUFFER_OPERATION\n");
				break;

			case GL_OUT_OF_MEMORY:
				gLogger.error("OpenGL error: GL_OUT_OF_MEMORY\n");
				break;

			case GL_STACK_UNDERFLOW:
				gLogger.error("OpenGL error: GL_STACK_UNDERFLOW\n");
				break;

			case GL_STACK_OVERFLOW:
				gLogger.error("OpenGL error: GL_STACK_OVERFLOW\n");
				break;
			}
		}
	}
}

Renderer::Renderer(SDLContext *context) : mContext(context) {

}

bool Renderer::init() {
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	mGLContext = SDL_GL_CreateContext(mContext->getGameWindow());
	SDL_GL_MakeCurrent(mContext->getGameWindow(), mGLContext);

	glewExperimental = 1;
	if (glewInit() != GLEW_OK)
		return false;

	PumpOpenGLErrors(false);

    bool shaderLoaded;
	m2DShader = new Shader();
	m2DShader->addInput("iPosition", 0);
	m2DShader->addInput("iTexCoord", 1);
	m2DShader->addInput("iColor", 2);
	shaderLoaded = m2DShader->loadFromFile("../Content/Shaders/2D.vert", "../Content/Shaders/2D.frag");
    assert(shaderLoaded);

	mFontShader = new Shader();
	mFontShader->addInput("iPosition", 0);
	mFontShader->addInput("iTexCoord", 1);
	shaderLoaded = mFontShader->loadFromFile("../Content/Shaders/Font.vert", "../Content/Shaders/Font.frag");
    assert(shaderLoaded);

	return true;
}

void Renderer::shutdown() {
	for (auto it : mTextureMap) {
		delete it.second;
	}
	mTextureMap.clear();

	for (SpriteBatcher *batcher : mSpriteBatchers) {
		delete batcher;
	}
	mSpriteBatchers.clear();

	delete mFontShader;
	delete m2DShader;
}

void Renderer::beginFrame() {
	glClear(GL_COLOR_BUFFER_BIT);

	glm::mat4 projMatrix = glm::ortho(0.0f, (float)mContext->getWindowWidth(), (float)mContext->getWindowHeight(), 0.0f, 0.0f, 1.0f);

	mFontShader->use();
	mFontShader->setUniformMat4("gTransform", projMatrix);

	m2DShader->use();
	m2DShader->setUniformMat4("gTransform", projMatrix);

	for (SpriteBatcher *batcher : mSpriteBatchers) {
		batcher->prepare();
	}
}

void Renderer::endFrame() {
	for (SpriteBatcher *batcher : mSpriteBatchers) {
		batcher->submit();
	}

	// mContext->getGUI()->render();

	SDL_GL_SwapWindow(mContext->getGameWindow());

	PumpOpenGLErrors();
}

Texture *Renderer::getTexture(const std::string &filename) {
	auto it = mTextureMap.find(filename);
	if (it != mTextureMap.end()) {
		return it->second;
	}

	Texture *texture = new Texture();
	if (texture->loadFromFile(filename)) {
		texture->incrementRefs();
		mTextureMap.insert({ filename, texture });
		return texture;
	}
	
	assert(false);
	return NULL;
}

void Renderer::freeUnreferencedTextures() {
	for (auto it = mTextureMap.begin(); it != mTextureMap.end();) {
		if (it->second->getRefCount() == 0) {
			Texture *texture = it->second;

			delete it->second;
			it = mTextureMap.erase(it);

			for (auto it2 = mSpriteBatchers.begin(); it2 != mSpriteBatchers.end();) {
				if (texture == (*it2)->getTexture()) {
					delete *it2;
					mSpriteBatchers.erase(it2);
				}
				else {
					++it2;
				}
			}
		}
		else {
			++it;
		}
	}
}

SpriteBatcher *Renderer::getSpriteBatcher(Texture *texture, SpriteBlendMode blendMode) {
	assert(texture != NULL);

	for (SpriteBatcher *batcher : mSpriteBatchers) {
		if (batcher->getTexture() == texture && batcher->getBlendMode() == blendMode) {
			return batcher;
		}
	}

	SpriteBatcher *batcher = new SpriteBatcher(texture, blendMode);
	batcher->prepare();

	mSpriteBatchers.push_back(batcher);

	return batcher;
}

