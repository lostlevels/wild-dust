#pragma once

#include "BlendMode.h"

class Client;
class Shader;
class Texture;
class SpriteBatcher;

class Renderer {
public:
	Renderer(Client *context);

	bool init();
	void shutdown();

	void beginFrame();
	void endFrame();

	Texture *getTexture(const std::string &filename);
	void freeUnreferencedTextures();

	Shader *getFontShader() { return mFontShader; }

	SpriteBatcher *getSpriteBatcher(Texture *texture, SpriteBlendMode blendMode);

private:
	Client *mContext;
	SDL_GLContext mGLContext;
	Shader *m2DShader;
	Shader *mFontShader;
	std::map<std::string, Texture*> mTextureMap;
	std::vector<SpriteBatcher*> mSpriteBatchers;
};
