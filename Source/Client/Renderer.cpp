#include "Precompiled.h"
#include "Renderer.h"
#include "Client.h"
#include "Texture.h"
#include "Shader.h"

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

Renderer::Renderer(Client *context) {
	mContext = context;
}

bool Renderer::init() {
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	mGLContext = SDL_GL_CreateContext(mContext->getGameWindow());
	SDL_GL_MakeCurrent(mContext->getGameWindow(), mGLContext);

	glewExperimental = 1;
	if (glewInit() != GLEW_OK)
		return false;

	PumpOpenGLErrors(false);

	m2DShader = new Shader();
	m2DShader->addInput("iPosition", 0);
	m2DShader->addInput("iTexCoord", 1);
	m2DShader->loadFromFile("../Content/Shaders/2D.vert", "../Content/Shaders/2D.frag");

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
	for (auto it : mTextureMap) {
		delete it.second;
	}
	mTextureMap.clear();

	delete m2DShader;

	glDeleteVertexArrays(1, &mVAO);
	glDeleteBuffers(1, &mIBO);
	glDeleteBuffers(1, &mVBO);
}

void Renderer::beginFrame() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projMatrix = glm::ortho(0.0f, (float)mContext->getWindowWidth(), (float)mContext->getWindowHeight(), 0.0f, 0.0f, 1.0f);

	m2DShader->use();
	m2DShader->setUniformMat4("gTransform", projMatrix);

	PumpOpenGLErrors();
}

void Renderer::endFrame() {
	SDL_GL_SwapWindow(mContext->getGameWindow());
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

	return NULL;
}

void Renderer::freeUnreferencedTextures() {
	for (auto it = mTextureMap.begin(); it != mTextureMap.end();) {
		if (it->second->getRefCount() == 0) {
			delete it->second;
			it = mTextureMap.erase(it);
		}
		else {
			++it;
		}
	}
}

void Renderer::drawQuad(Texture *texture, const Vec2 &position, float scale, float z) {
	float verts[] = {
		position.x * scale, position.y * scale, z,
		0.0f, 0.0f,

		(position.x + texture->getWidth()) * scale, position.y * scale, z,
		1.0f, 0.0f,

		(position.x + texture->getWidth()) * scale, (position.y + texture->getHeight()) * scale, z,
		1.0f, 1.0f,

		position.x * scale, (position.y + texture->getHeight()) * scale, z,
		0.0f, 1.0f
	};

	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);

	texture->use(0);

	glBindVertexArray(mVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
	glBindVertexArray(0);
}

void Renderer::drawQuad(Texture *texture, const Vec2 &position, const Recti &source, float scale, float z) {
	float uvLeft = (float)source.x / (float)texture->getWidth();
	float uvTop = (float)source.y / (float)texture->getHeight();
	float uvRight = uvLeft + (float)source.w / (float)texture->getWidth();
	float uvBottom = uvTop + (float)source.h / (float)texture->getHeight();

	float verts[] = {
		(position.x  * scale) * scale, position.y  * scale, z,
		uvLeft, uvTop,

		(position.x + source.w) * scale, position.y * scale, z,
		uvRight, uvTop,

		(position.x + source.w) * scale, (position.y + source.h) * scale, z,
		uvRight, uvBottom,

		position.x * scale, (position.y + source.h) * scale, z,
		uvLeft, uvBottom
	};

	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);

	texture->use(0);

	glBindVertexArray(mVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
	glBindVertexArray(0);
}
