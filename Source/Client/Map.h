#pragma once

#define TILE_WIDTH 16
#define TILE_HEIGHT 16

class Client;
class Texture;
class ClientMap;
class PhysicsObject;

class ClientMapTileset {
public:
	ClientMapTileset(ClientMap *map);
	~ClientMapTileset();
	
	bool loadTexture(const std::string &filename);

	Texture *getTexture() const { return mTexture; }
	int getWidthInTiles() const { return mWidthInTiles; }
	int getHeightInTiles() const { return mHeightInTiles; }

private:
	ClientMap *mMap;
	Texture *mTexture;
	int mWidthInTiles;
	int mHeightInTiles;
};

class ClientMapLayer {
public:
	ClientMapLayer(int width, int height);
	~ClientMapLayer();

	int getTileIndex(int x, int y) const { return mTileIndices[y*mWidth + x]; }
	int getTilesetIndex(int x, int y) const { return mTilesetIndices[y*mWidth + x]; }
	void setTileIndex(int x, int y, int idx) { mTileIndices[y*mWidth + x] = idx; }
	void setTilesetIndex(int x, int y, int idx) { mTilesetIndices[y*mWidth + x] = idx; }

private:
	int mWidth;
	int mHeight;
	int8_t *mTileIndices;
	int8_t *mTilesetIndices;
};

class ClientMap {
public:
	ClientMap(Client *client);
	~ClientMap();

	Client *getClient() { return mClient; }

	bool loadFromFile(const std::string &mapName);
	void unload();
	bool isLoaded() const { return mIsLoaded; }
	std::string getName() const { return mName; }

	void draw() const;

private:
	void createCollider(int x, int y, int w, int h);

private:
	Client *mClient;
	bool mIsLoaded;
	std::string mName;
	std::vector<ClientMapTileset*> mTilesets;
	std::vector<ClientMapLayer*> mLayers;
	std::vector<PhysicsObject*> mColliders;
	int mWidth;
	int mHeight;
};