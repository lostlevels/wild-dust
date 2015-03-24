#pragma once

class Renderer;
class Font;

class GUI {
public:
	GUI(Renderer *renderer);
	~GUI();

	bool init();
	void shutdown();

	void render();

private:
	Renderer *mRenderer;
	Font *mVera;
};