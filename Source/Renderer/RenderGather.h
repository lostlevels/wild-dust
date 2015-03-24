#pragma once

#include "BlendMode.h"

class World;
class Renderer;

// This gathers entities from World and prepares it for Renderer since not the responsibility of the renderer to
// gather
class RenderGather {
public:
	void drawWorld(World *world, Renderer *renderer);
};
