#include "Precompiled.h"
#include "AnimationRenderer.h"
#include "Shared/AnimationSheet.h"
#include "Renderer.h"
#include "Texture.h"
#include "SpriteBatcher.h"

AnimationRenderer::AnimationRenderer(AnimationSheet *sheet, Renderer *renderer) {
	mSheet = sheet;
	mRenderer = renderer;
	mTexture = mRenderer->getTexture(std::string("../Content/Textures/") + sheet->getImageFilename());
}

AnimationRenderer::~AnimationRenderer() {
	mTexture->decrementRefs();
}

void AnimationRenderer::render(Animation *anim, const Vec2 &position, const Vec2 &size, const Color &tint) {
	int framesPerRow = mTexture->getWidth() / mSheet->getFrameWidth();

	int currentFrame = anim->getCurrentFrame().index;
	int currentFrameX = currentFrame % framesPerRow;
	int currentFrameY = currentFrame / framesPerRow;

	Recti source;
	source.x = currentFrameX * mSheet->getFrameWidth();
	source.y = currentFrameY * mSheet->getFrameHeight();
	source.w = mSheet->getFrameWidth();
	source.h = mSheet->getFrameHeight();

	uint8_t flipFlags = 0;
	if (anim->getFlipX()) {
		flipFlags |= FLIP_H;
	}
	if (anim->getFlipY()) {
		flipFlags |= FLIP_V;
	}

	SpriteBatcher *batcher = mRenderer->getSpriteBatcher(mTexture, BLEND_ALPHA);
	batcher->addSprite(position, size, source, tint, flipFlags);

}

