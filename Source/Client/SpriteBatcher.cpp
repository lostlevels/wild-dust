#include "Precompiled.h"
#include "SpriteBatcher.h"
#include "Texture.h"

#define MAX_QUADS 64000

SpriteBatcher::SpriteBatcher(Texture *texture, SpriteBlendMode blendMode) {
	mTexture = texture;
	mBlendMode = blendMode;
	mSpriteCount = 0;

	glGenBuffers(3, mVBOs);
	glGenBuffers(3, mIBOs);
	glGenVertexArrays(3, mVAOs);

	for (int i = 0; i < 3; ++i) {
		glBindVertexArray(mVAOs[i]);

		glBindBuffer(GL_ARRAY_BUFFER, mVBOs[i]);
		glBufferData(GL_ARRAY_BUFFER, MAX_QUADS * sizeof(SpriteVertex) * 4, NULL, GL_STREAM_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBOs[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_QUADS * sizeof(uint32_t) * 6, NULL, GL_STREAM_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), (char*)NULL + 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), (char*)NULL + 8);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), (char*)NULL + 16);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);
	}

	mActiveBufferIndex = 0;
}

SpriteBatcher::~SpriteBatcher() {
	glDeleteVertexArrays(3, mVAOs);
	glDeleteBuffers(3, mVBOs);
	glDeleteBuffers(3, mIBOs);
}

void SpriteBatcher::prepare() {
	mSpriteCount = 0;

	glBindBuffer(GL_ARRAY_BUFFER, mVBOs[mActiveBufferIndex]);
	//mVertexData = (SpriteVertex*)glMapBufferRange(GL_ARRAY_BUFFER, 0, MAX_QUADS * sizeof(SpriteVertex) * 4, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
	mVertexData = (SpriteVertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBOs[mActiveBufferIndex]);
	//mIndexData = (uint32_t*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, MAX_QUADS * sizeof(uint32_t) * 6, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
	mIndexData = (uint32_t*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
}

void SpriteBatcher::submit() {
	glBindBuffer(GL_ARRAY_BUFFER, mVBOs[mActiveBufferIndex]);
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBOs[mActiveBufferIndex]);
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	if (mSpriteCount > 0) {
		switch (mBlendMode) {
		case BLEND_ALPHA:
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;

		case BLEND_ADDITIVE:
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);
			break;

		case BLEND_NONE:
			glDisable(GL_BLEND);
			break;
		}

		mTexture->use(0);

		glBindVertexArray(mVAOs[mActiveBufferIndex]);
		glDrawElements(GL_TRIANGLES, 6 * mSpriteCount, GL_UNSIGNED_INT, NULL);
		glBindVertexArray(0);

		if (++mActiveBufferIndex >= 3) {
			mActiveBufferIndex = 0;
		}
	}
}

void SpriteBatcher::addSprite(const Vec2 &position, const Vec2 &size, const Recti &source, const Color &tint) {
	SpriteVertex *v;

	float uvLeft = (float)source.x / (float)mTexture->getWidth();
	float uvTop = (float)source.y / (float)mTexture->getHeight();
	float uvRight = uvLeft + (float)source.w / (float)mTexture->getWidth();
	float uvBottom = uvTop + (float)source.h / (float)mTexture->getHeight();

	v = mVertexData;
	v->x = position.x;
	v->y = position.y;
	v->u = uvLeft;
	v->v = uvTop;
	v->r = tint.r;
	v->g = tint.g;
	v->b = tint.b;
	v->a = tint.a;
	++mVertexData;

	v = mVertexData;
	v->x = position.x + size.x;
	v->y = position.y;
	v->u = uvRight;
	v->v = uvTop;
	v->r = tint.r;
	v->g = tint.g;
	v->b = tint.b;
	v->a = tint.a;
	++mVertexData;

	v = mVertexData;
	v->x = position.x + size.x;
	v->y = position.y + size.y;
	v->u = uvRight;
	v->v = uvBottom;
	v->r = tint.r;
	v->g = tint.g;
	v->b = tint.b;
	v->a = tint.a;
	++mVertexData;

	v = mVertexData;
	v->x = position.x;
	v->y = position.y + size.y;
	v->u = uvLeft;
	v->v = uvBottom;
	v->r = tint.r;
	v->g = tint.g;
	v->b = tint.b;
	v->a = tint.a;
	++mVertexData;

	int baseIndex = mSpriteCount * 4;

	*mIndexData = baseIndex + 0;
	++mIndexData;

	*mIndexData = baseIndex + 1;
	++mIndexData;

	*mIndexData = baseIndex + 2;
	++mIndexData;

	*mIndexData = baseIndex + 3;
	++mIndexData;

	*mIndexData = baseIndex + 0;
	++mIndexData;

	*mIndexData = baseIndex + 2;
	++mIndexData;

	++mSpriteCount;
}

void SpriteBatcher::addSprite(const Vec2 &position, const Recti &source, const Color &tint) {
	addSprite(position, Vec2(source.w, source.h), source, tint);
}

void SpriteBatcher::addSprite(const Vec2 &position, const Color &tint) {
	Recti source;
	source.x = 0;
	source.y = 0;
	source.w = mTexture->getWidth();
	source.h = mTexture->getHeight();
	addSprite(position, source, tint);
}
