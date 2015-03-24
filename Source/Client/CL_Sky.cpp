#include "Precompiled.h"
#include "CL_Sky.h"
#include "Texture.h"
#include "Client.h"
#include "Renderer.h"
#include "SpriteBatcher.h"
#include <cmath>

CL_Sky::CL_Sky(Client *client) : CL_Entity(client), mTwinkleTime(0) {
	mTexture = client->getRenderer()->getTexture("../Content/Textures/Misc/Star.png");

	int numStars = 25;
	mStarPositions.resize(numStars);
	int windowWidth = mClient->getWindowWidth();
	int windowHeight = mClient->getWindowHeight();
	for (int i = 0; i < numStars; ++i) {
		Vec2 position;
		position.x = rand() % windowWidth;
		position.y = rand() % windowHeight;
		mStarPositions[i] = position;
	}
}

CL_Sky::~CL_Sky() {
	mTexture->decrementRefs();
}

void CL_Sky::update(float dt) {
	CL_Entity::update(dt);

	mTwinkleTime += dt;
}

void CL_Sky::draw() {
	float maxExtraSize = 4;
	float extraSize = std::sinf(mTwinkleTime * 3.1419265f) * maxExtraSize;

	Vec2 size(16 + extraSize, 16 + extraSize);

	Recti source;
	source.x = 0;
	source.y = 0;
	source.w = mTexture->getWidth();
	source.h = mTexture->getHeight();
	Renderer *renderer = mClient->getRenderer();
	SpriteBatcher *batcher = renderer->getSpriteBatcher(mTexture, BLEND_ALPHA);

	for (std::size_t i = 0; i < mStarPositions.size(); ++i) {
		Vec2 position = mStarPositions[i];
		position.x += -extraSize / 2;
		position.y += -extraSize / 2;
		batcher->addSprite(position, size, source, Color(1.0f), FLIP_NONE);
	}
}
