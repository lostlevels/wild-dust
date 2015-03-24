#pragma once

class AnimationSheet;
class Animation;
class Renderer;
class Texture;

class AnimationRenderer {
public:
	AnimationRenderer(AnimationSheet *sheet, Renderer *render);
	~AnimationRenderer();

	void render(Animation *anim, const Vec2 &position, const Vec2 &size, const Color &tint);

private:
	AnimationSheet *mSheet;
	Renderer *mRenderer;
	Texture *mTexture;
};
