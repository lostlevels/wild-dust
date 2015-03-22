#pragma once

#include "BlendMode.h"

class Shader;
class Texture;
class SpriteBatcher;

class RendererBase {
public:
	RendererBase(SDL_Window *window, int screenWidth, int screenHeight);

	bool init();
	void shutdown();

	void beginFrame();
	void endFrame();

	Texture *getTexture(const std::string &filename);
	void freeUnreferencedTextures();

	Shader *getFontShader() { return mFontShader; }

	SpriteBatcher *getSpriteBatcher(Texture *texture, SpriteBlendMode blendMode);

private:
	SDL_Window *mWindow;
	int mScreenWidth;
	int mScreenHeight;

	SDL_GLContext mGLContext;
	Shader *m2DShader;
	Shader *mFontShader;
	std::map<std::string, Texture*> mTextureMap;
	std::vector<SpriteBatcher*> mSpriteBatchers;
};
