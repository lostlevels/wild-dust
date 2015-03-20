#include "Precompiled.h"
#include "Renderer.h"
#include "Client.h"
#include "Texture.h"
#include "Shader.h"

Renderer::Renderer(Client *context) {
	mContext = context;
}

bool Renderer::init() {
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

	mGLContext = SDL_GL_CreateContext(mContext->getGameWindow());
	SDL_GL_MakeCurrent(mContext->getGameWindow(), mGLContext);

	glewExperimental = 1;
	if (glewInit() != GLEW_OK)
		return false;

	m2DShader = new Shader();
	m2DShader->addInput("iPosition", 0);
	m2DShader->addInput("iTexCoord", 1);
	m2DShader->loadFromFile("../Content/Shaders/2D.vert", "../Content/Shaders/2D.frag");

	mGrass = new Texture();
	mGrass->loadFromFile("../Content/Textures/Grass.jpg");

	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);

	uint16_t inds[] = {
		0, 1, 2,
		3, 0, 2
	};

	glGenBuffers(1, &mIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(inds), inds, GL_STATIC_DRAW);

	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 20, (char*)NULL + 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 20, (char*)NULL + 12);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	return true;
}

void Renderer::shutdown() {
	delete mGrass;
	delete m2DShader;
}

void Renderer::beginFrame() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mProjMatrix = glm::ortho(0.0f, (float)mContext->getWindowWidth(), (float)mContext->getWindowHeight(), 0.0f, 0.0f, 1.0f);

	m2DShader->use();
	m2DShader->setUniformMat4("gTransform", mProjMatrix);

	mGrass->use(0);

	drawQuad(mGrass, Vec2(0.0f, 0.0f), 0.0f, 1.0f, 0.0f);
}

void Renderer::endFrame() {
	SDL_GL_SwapWindow(mContext->getGameWindow());
}

void Renderer::drawQuad(Texture *texture, const Vec2 &position, float rotation, float scale, float z) {
	float verts[] = {
		position.x, position.y, z,
		0.0f, 0.0f,

		position.x + texture->getWidth(), position.y, z,
		1.0f, 0.0f,

		position.x + texture->getWidth(), position.y + texture->getHeight(), z,
		1.0f, 1.0f,

		position.x, position.y + texture->getHeight(), z,
		0.0f, 1.0f
	};

	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);

	glBindVertexArray(mVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
	glBindVertexArray(0);
}

void Renderer::drawQuad(Texture *texture, const Vec2 &position, const Recti &source, float rotation, float scale, float z) {
	float uvLeft = (float)source.x / (float)texture->getWidth();
	float uvTop = (float)source.y / (float)texture->getHeight();
	float uvRight = uvLeft + (float)source.w / (float)texture->getWidth();
	float uvBottom = uvTop + (float)source.h / (float)texture->getHeight();

	float verts[] = {
		position.x, position.y, z,
		uvLeft, uvTop,

		position.x + source.w, position.y, z,
		uvRight, uvTop,

		position.x + source.w, position.y + source.h, z,
		uvRight, uvBottom,

		position.x, position.y + source.h, z,
		uvLeft, uvBottom
	};

	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);

	glBindVertexArray(mVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
	glBindVertexArray(0);
}
