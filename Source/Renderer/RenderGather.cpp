#include "Precompiled.h"
#include "RenderGather.h"
#include "Texture.h"
#include "Shader.h"
#include "SpriteBatcher.h"
#include "Font.h"
#include "Renderer.h"
#include "GUI.h"
#include "Core/Entity.h"
#include "Core/TilemapEntity.h"
#include "Core/GibCollectionEntity.h"
#include "Core/StarsEntity.h"
#include "Core/World.h"
#include <Tmx.h>

static std::string getTilesetImagePath(const Tmx::Tileset *tileset) {
	return std::string("../Content/Maps/") + tileset->GetImage()->GetSource();
}

static std::string getObjectImagePath(const Tmx::Object *object) {
	return std::string("../Content/Textures/") + object->GetProperties().GetStringProperty("sprite");
}

void RenderGather::drawWorld(World *world, Renderer *renderer, GUI *gui, const std::vector<GUIData> &data) {
	renderer->beginFrame();

	auto entities = world->getEntities();
	for (auto &kv : entities) {
		auto ent = kv.second;
		if (!ent->getActive()) continue;

		auto renderType = ent->getRenderType();
		if (renderType == "sprite")
			drawSprite(ent, renderer);
		else if (renderType == "tilemap")
			drawTilemap(ent, renderer);
		else if (renderType == "gibs")
			drawGibs(ent, renderer);
		else if (renderType == "stars")
			drawStars(ent, renderer);
	}

	if (gui) gui->setGUIData(data);

	renderer->endFrame(gui);
}

void RenderGather::drawStars(Entity *ent, Renderer *renderer) {
	auto starEnt = static_cast<StarsEntity*>(ent);
	auto &stars = starEnt->getStars();

	auto &rep = ent->getRepresentation();
	auto texture = renderer->getTexture(rep.imageFile);
	auto batcher = renderer->getSpriteBatcher(texture, BLEND_ALPHA);
	Recti source = {0, 0, texture->getWidth(), texture->getHeight()};

	for (auto &star : stars) {
		Vec2 position(star.centerPosition.x - star.size.x/2, star.centerPosition.y - star.size.y/2);
		batcher->addSprite(position, star.size, source, Color(1.0f), 0);
	}
}

void RenderGather::drawGibs(Entity *ent, Renderer *renderer) {
	auto gibCollection = static_cast<GibCollectionEntity*>(ent);
	int numGibs = gibCollection->getNumGibs();
	const int *gibIndices = gibCollection->getGibIndices();
	const Gib *gibs = gibCollection->getGibs();

	auto &rep = ent->getRepresentation();
	auto texture = renderer->getTexture(rep.imageFile);
	auto batcher = renderer->getSpriteBatcher(texture, BLEND_ALPHA);
	Recti source = {0, 0, texture->getWidth(), texture->getHeight()};
	for (int i = 0; i < numGibs; ++i) {
		int index = gibIndices[i];
		const auto &gib = gibs[index];

		batcher->addSprite(Vec2(gib.position), gib.size, source, Color(1.0f), 0);
	}
}

void RenderGather::drawSprite(Entity *ent, Renderer *renderer) {
	auto &rep = ent->getRepresentation();
	std::string textureFile = rep.imageFile;

	if (!textureFile.empty()) {
		// Note assumes vertical framestrip
		// If animation / frame, calculate the animated frame
		int frame = ent->getFrame();
		Recti source = rep.source;
		if (frame > -1) source = {0, rep.source.h * frame, rep.source.w, rep.source.h};

		auto texture = renderer->getTexture(textureFile);
		auto batcher = renderer->getSpriteBatcher(texture, BLEND_ALPHA);

		batcher->addSprite(Vec2(ent->getPosition()), rep.size, source, Color(1.0f), ent->getFlip() ? FLIP_H : 0);
	}
}

void RenderGather::drawTilemap(Entity *ent, Renderer *renderer) {
	auto tilemap = static_cast<TilemapEntity*>(ent);
	auto &tmxMap = tilemap->getMap();

	int width = tmxMap.GetWidth();
	int height = tmxMap.GetHeight();
	int tileWidth = tmxMap.GetTileWidth();
	int tileHeight = tmxMap.GetTileHeight();

	for (int i = 0; i < tmxMap.GetNumTileLayers(); ++i) {
		auto tmxLayer = tmxMap.GetTileLayer(i);
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {

				int tileId = tmxLayer->GetTileId(x, y);
				int tilesetIndex = tmxLayer->GetTileTilesetIndex(x, y);
				if (tileId == -1 || tilesetIndex == -1) continue;

				auto tileset = tmxMap.GetTileset(tilesetIndex);
				auto texture = renderer->getTexture(getTilesetImagePath(tileset));
				auto batcher = renderer->getSpriteBatcher(texture, BLEND_ALPHA);
				int widthInTiles = texture->getWidth() / tileWidth;

				Vec2 position(tileWidth * x, tileHeight * y);
				Recti tileSource;
				tileSource.x = (tileId % widthInTiles) * tileWidth;
				tileSource.y = (tileId / widthInTiles) * tileHeight;
				tileSource.w = tileWidth;
				tileSource.h = tileHeight;

				batcher->addSprite(position, tileSource, Color(1.0f), 0);
			}
		}
	}

	for (int i = 0; i < tmxMap.GetNumObjectGroups(); ++i) {
		auto tmxGroup = tmxMap.GetObjectGroup(i);
		for (int j = 0; j < tmxGroup->GetNumObjects(); ++j) {
			auto tmxObject = tmxGroup->GetObject(j);

			std::string spriteName = getObjectImagePath(tmxObject);
			auto texture = renderer->getTexture(spriteName);
			auto batcher = renderer->getSpriteBatcher(texture, BLEND_ALPHA);

			Vec2 position(tmxObject->GetX(), tmxObject->GetY());
			Vec2 size(tmxObject->GetWidth(), tmxObject->GetHeight());
			Recti source = {0, 0, texture->getWidth(), texture->getHeight()};
			batcher->addSprite(position, size, source, Color(1.0f), 0);
		}
	}
}
