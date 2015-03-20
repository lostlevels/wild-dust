#pragma once

#include "BlendMode.h"

class Texture;

struct SpriteVertex {
	float x;
	float y;
	float u;
	float v;
	float r;
	float g;
	float b;
	float a;
};

class SpriteBatcher {
public:
	SpriteBatcher(Texture *texture, SpriteBlendMode blendMode);
	~SpriteBatcher();

	// Prepare the batcher for adding new sprites
	void prepare();
	// Submit all added sprites to the GPU in one efficient draw call
	void submit();

	// Adds sprites to the batch, they will be cleared out the next frame
	void addSprite(const Vec2 &position, const Recti &source, const Color &tint);
	void addSprite(const Vec2 &position, const Color &tint);

	Texture *getTexture() const { return mTexture; }
	SpriteBlendMode getBlendMode() const { return mBlendMode; }

private:
	Texture *mTexture;
	SpriteBlendMode mBlendMode;
	int mSpriteCount;
	GLuint mVAOs[3];
	GLuint mVBOs[3];
	GLuint mIBOs[3];
	int mActiveBufferIndex;
	SpriteVertex *mVertexData;
	uint32_t *mIndexData;
};