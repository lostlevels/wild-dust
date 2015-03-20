#pragma once

class Client;
class Shader;
class Texture;

class Renderer {
public:
	Renderer(Client *context);

	bool init();
	void shutdown();

	void beginFrame();
	void endFrame();

	Texture *getTexture(const std::string &filename);
	void freeUnreferencedTextures();

	void drawQuad(Texture *texture, const Vec2 &position, float scale, float z);
	void drawQuad(Texture *texture, const Vec2 &position, const Recti &source, float scale, float z);

private:
	Client *mContext;
	SDL_GLContext mGLContext;
	Shader *m2DShader;
	GLuint mVAO;
	GLuint mVBO;
	GLuint mIBO;
	std::map<std::string, Texture*> mTextureMap;
};
