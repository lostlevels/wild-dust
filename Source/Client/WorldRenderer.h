#pragma once

#include "BlendMode.h"

class World;
class RendererBase;

// Todo: call this something better, basically this gathers entities from World and prepares it for Renderer
class WorldRenderer {
public:
	void drawWorld(World *world, RendererBase *renderer);
};
