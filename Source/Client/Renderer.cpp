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

	glm::vec2 verts[] = {
		glm::vec2(100.0f, 100.0f),
		glm::vec2(0, 0),

		glm::vec2(200.0f, 100.0f),
		glm::vec2(1, 0),

		glm::vec2(200.0f, 200.0f),
		glm::vec2(1, 1),

		glm::vec2(100.0f, 200.0f),
		glm::vec2(0, 1),
	};

	uint16_t inds[] = {
		0, 1, 2,
		3, 0, 2
	};

	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	glGenBuffers(1, &mIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(inds), inds, GL_STATIC_DRAW);

	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 16, (char*)NULL + 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 16, (char*)NULL + 8);
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
	glClear(GL_COLOR_BUFFER_BIT);

	mProjMatrix = glm::ortho(0.0f, (float)mContext->getWindowWidth(), (float)mContext->getWindowHeight(), 0.0f, 0.0f, 1.0f);

	m2DShader->use();
	m2DShader->setUniformMat4("gTransform", mProjMatrix);

	mGrass->use(0);

	glBindVertexArray(mVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
	glBindVertexArray(0);
}

void Renderer::endFrame() {
	SDL_GL_SwapWindow(mContext->getGameWindow());
}
