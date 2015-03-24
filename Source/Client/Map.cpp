#include "Precompiled.h"
#include "Map.h"
#include "Client.h"
#include "Texture.h"
#include "Renderer.h"
#include "SpriteBatcher.h"
#include <Tmx.h>
#include "Physics/Physics.h"
#include "Physics/PhysicsObject.h"

ClientMapTileset::ClientMapTileset(ClientMap *map) {
	mMap = map;
	mTexture = NULL;
	mWidthInTiles = 0;
	mHeightInTiles = 0;
}

ClientMapTileset::~ClientMapTileset() {
	if (mTexture) {
		mTexture->decrementRefs();
	}
}

bool ClientMapTileset::loadTexture(const std::string &filename) {
	if (mTexture) {
		mTexture->decrementRefs();
	}
	mTexture = mMap->getClient()->getRenderer()->getTexture(filename);
	mWidthInTiles = mTexture->getWidth() / TILE_WIDTH;
	mHeightInTiles = mTexture->getHeight() / TILE_HEIGHT;
	return mTexture != NULL;
}

ClientMapLayer::ClientMapLayer(int width, int height) {
	mWidth = width;
	mHeight = height;
	mTileIndices = new int8_t[width*height];
	mTilesetIndices = new int8_t[width*height];
}

ClientMapLayer::~ClientMapLayer() {
	delete[]mTileIndices;
	delete[]mTilesetIndices;
}

ClientMap::ClientMap(Client *client) {
	mClient = client;
	mIsLoaded = false;
}

ClientMap::~ClientMap() {
	unload();
}

bool ClientMap::loadFromFile(const std::string &mapName) {
	Tmx::Map tmxMap;
	tmxMap.ParseFile(std::string("../Content/Maps/") + mapName);

	if (tmxMap.HasError()) {
		gLogger.error("Could not load map '%s`: %s\n", mapName.c_str(), tmxMap.GetErrorText());
		return false;
	}

	unload();

	mWidth = tmxMap.GetWidth();
	mHeight = tmxMap.GetHeight();

	for (int i = 0; i < tmxMap.GetNumTilesets(); ++i) {
		const Tmx::Tileset *tmxTileset = tmxMap.GetTileset(i);
		ClientMapTileset *tileset = new ClientMapTileset(this);
		tileset->loadTexture(std::string("../Content/Maps/") + tmxTileset->GetImage()->GetSource());
		mTilesets.push_back(tileset);
	}

	for (int i = 0; i < tmxMap.GetNumTileLayers(); ++i) {
		const Tmx::TileLayer *tmxLayer = tmxMap.GetTileLayer(i);
		ClientMapLayer *layer = new ClientMapLayer(mWidth, mHeight);
		for (int x = 0; x < mWidth; ++x) {
			for (int y = 0; y < mHeight; ++y) {
				layer->setTileIndex(x, y, tmxLayer->GetTileId(x, y));
				layer->setTilesetIndex(x, y, tmxLayer->GetTileTilesetIndex(x, y));
			}
		}
		mLayers.push_back(layer);
	}

	for (int i = 0; i < tmxMap.GetNumObjectGroups(); ++i) {
		const Tmx::ObjectGroup *tmxGroup = tmxMap.GetObjectGroup(i);
		for (int j = 0; j < tmxGroup->GetNumObjects(); ++j) {
			const Tmx::Object *tmxObject = tmxGroup->GetObject(j);
			createCollider(
				tmxObject->GetX(),
				tmxObject->GetY(),
				tmxObject->GetWidth(),
				tmxObject->GetHeight());
		}
	}

	gLogger.info("Loaded map '%s` successfully.\n", mapName.c_str());

	mName = mapName;
	mIsLoaded = true;

	return true;
}

void ClientMap::unload() {
	for (ClientMapLayer *layer : mLayers) {
		delete layer;
	}
	mLayers.clear();

	for (ClientMapTileset *tileset : mTilesets) {
		delete tileset;
	}
	mTilesets.clear();

	for (PhysicsObject *collider : mColliders) {
		delete collider;
	}
	mColliders.clear();

	mIsLoaded = false;
}

void ClientMap::draw() const {
	Renderer *renderer = mClient->getRenderer();

	SpriteBatcher *batchers[32];
	for (std::size_t i = 0; i < mTilesets.size(); ++i) {
		batchers[i] = renderer->getSpriteBatcher(mTilesets[i]->getTexture(), BLEND_ALPHA);
	}

	for (ClientMapLayer *layer : mLayers) {
		for (int x = 0; x < mWidth; ++x) {
			for (int y = 0; y < mHeight; ++y) {
				int tilesetID = layer->getTilesetIndex(x, y);
				if (tilesetID == -1) {
					continue;
				}

				int tileID = layer->getTileIndex(x, y);
				
				Recti tileSource;
				tileSource.x = (tileID % mTilesets[tilesetID]->getWidthInTiles()) * TILE_WIDTH;
				tileSource.y = (tileID / mTilesets[tilesetID]->getWidthInTiles()) * TILE_HEIGHT;
				tileSource.w = TILE_WIDTH;
				tileSource.h = TILE_HEIGHT;
				
				batchers[tilesetID]->addSprite(Vec2(x*TILE_WIDTH, y*TILE_HEIGHT), tileSource, Color(1.0f), FLIP_NONE);
			}
		}
	}
}

void ClientMap::createCollider(int x, int y, int w, int h) {
	Physics *phys = mClient->getPhysics();
	PhysicsObject *object = phys->createObject(PHYSICS_STATIC);
	object->setBox(w, h);
	object->setPosition(Vec2(x, y));
	object->setFriction(100.0f);
	mColliders.push_back(object);
}

