#pragma once

#include "BlendMode.h"
#include "SDLContext.h"

class Shader;
class Texture;
class SpriteBatcher;

// Would subclass from IRenderer / RendererBase but no time for now, more refactor later.
class Renderer {
public:
	Renderer(SDLContext *context);

	bool init();
	void shutdown();

	void beginFrame();
	void endFrame();

	Texture *getTexture(const std::string &filename);
	void freeUnreferencedTextures();

	Shader *getFontShader() { return mFontShader; }

	SpriteBatcher *getSpriteBatcher(Texture *texture, SpriteBlendMode blendMode);

private:
	SDLContext *mContext;

	SDL_GLContext mGLContext;
	Shader *m2DShader;
	Shader *mFontShader;
	std::map<std::string, Texture*> mTextureMap;
	std::vector<SpriteBatcher*> mSpriteBatchers;
};
