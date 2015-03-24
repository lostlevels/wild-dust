#include "Precompiled.h"
#include "CL_Sky.h"
#include "Texture.h"
#include "Client.h"
#include "Renderer.h"
#include "SpriteBatcher.h"
#include <cmath>

CL_Sky::CL_Sky(Client *client) : CL_Entity(client) {
	mStarTexture = client->getRenderer()->getTexture("../Content/Textures/Misc/Star.png");
	mMoonTexture = client->getRenderer()->getTexture("../Content/Textures/Misc/Moon.png");

	int numStars = 25;
	mStarPositions.resize(numStars);
	mStarScales.resize(numStars);

	int windowWidth = mClient->getWindowWidth();
	int windowHeight = mClient->getWindowHeight();
	for (int i = 0; i < numStars; ++i) {
		Vec2 position;
		position.x = rand() % windowWidth;
		position.y = rand() % windowHeight;
		mStarPositions[i] = position;
		mStarScales[i] = Vec2(0, (rand()%100)/100 * 2 + 1.0f);
	}
}

CL_Sky::~CL_Sky() {
	mStarTexture->decrementRefs();
	mMoonTexture->decrementRefs();
}

void CL_Sky::update(float dt) {
	CL_Entity::update(dt);

	for (std::size_t i = 0; i < mStarScales.size(); ++i) {
		mStarScales[i].x += dt * mStarScales[i].y;
	}
}

void CL_Sky::draw() {
	drawStars();
	drawMoon();
}

void CL_Sky::drawStars() {
	float maxExtraSize = 4;

	Recti source;
	source.x = 0;
	source.y = 0;
	source.w = mStarTexture->getWidth();
	source.h = mStarTexture->getHeight();
	Renderer *renderer = mClient->getRenderer();
	SpriteBatcher *batcher = renderer->getSpriteBatcher(mStarTexture, BLEND_ALPHA);

	for (std::size_t i = 0; i < mStarPositions.size(); ++i) {
		float extraSize = std::sinf(mStarScales[i].x * 3.1419265f) * maxExtraSize;
		Vec2 size(16 + extraSize, 16 + extraSize);

		Vec2 position = mStarPositions[i];
		position.x += -extraSize / 2;
		position.y += -extraSize / 2;
		batcher->addSprite(position, size, source, Color(1.0f), FLIP_NONE);
	}
}

void CL_Sky::drawMoon() {
	int size = 500;
	Recti source;
	source.x = 0;
	source.y = 0;
	source.w = mMoonTexture->getWidth();
	source.h = mMoonTexture->getHeight();
	Renderer *renderer = mClient->getRenderer();
	SpriteBatcher *batcher = renderer->getSpriteBatcher(mMoonTexture, BLEND_ALPHA);
	batcher->addSprite(Vec2(500, mClient->getWindowHeight() - size * 1.75f), Vec2(size, size), source, Color(1.0f), FLIP_NONE);
}