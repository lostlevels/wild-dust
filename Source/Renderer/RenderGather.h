#pragma once

#include "BlendMode.h"

class World;
class Renderer;
class Entity;

// This gathers entities from World and prepares it for Renderer since not the responsibility of the renderer to
// gather
class RenderGather {
public:
	void drawWorld(World *world, Renderer *renderer);

private:
	void drawSprite(Entity *ent, Renderer *renderer);
	void drawTilemap(Entity *ent, Renderer *renderer);
	void drawGibs(Entity *ent, Renderer *renderer);
};
