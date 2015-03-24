#include "Precompiled.h"
#include "RenderGather.h"
#include "Texture.h"
#include "Shader.h"
#include "SpriteBatcher.h"
#include "Font.h"
#include "Renderer.h"
#include "Core/Entity.h"
#include "Core/World.h"
#include "GUI.h"

void RenderGather::drawWorld(World *world, Renderer *renderer) {
	renderer->beginFrame();

	auto entities = world->getEntities();
	for (auto &kv : entities) {
		auto entity = kv.second;
		auto &rep = entity->getRepresentation();
		std::string textureFile = rep.imageFile;

		if (!textureFile.empty()) {
			auto texture = renderer->getTexture(textureFile);
			auto batcher = renderer->getSpriteBatcher(texture, BLEND_ALPHA);
			batcher->addSprite(Vec2(entity->getPosition()), rep.source, Color(1.0f));
		}
	}

	renderer->endFrame();
}
