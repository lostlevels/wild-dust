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

	void render(const std::vector<GUIData> &data);
	void render();

	void setGUIData(const std::vector<GUIData> &data) { mData = data; }

private:
	Renderer *mRenderer;
	Font *mVera;
	std::vector<GUIData> mData;
};