#pragma once

#include "GUIData.h"
#include <vector>
class Renderer;
class Font;

class RENDERER_API GUI {
public:
	GUI(Renderer *renderer);
	~GUI();

	bool init();
	void shutdown();

	void render(const std::vector<GUIData> &datas);

private:
	Renderer *mRenderer;
	Font *mVera;
};