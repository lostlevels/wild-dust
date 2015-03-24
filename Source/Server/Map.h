#pragma once

class Server;
class PhysicsObject;

class ServerMap {
public:
	ServerMap(Server *server);
	~ServerMap();

	bool loadFromFile(const std::string &filename);
	bool isLoaded() const { return mIsLoaded; }
	std::string getName() const { return mName; }

	Vec2 getCSpawn() { return mCSpawn; }
	Vec2 getBSpawn() { return mBSpawn;  }

private:
	void createCollider(int x, int y, int w, int h);
	void unload();

private:
	Server *mServer;
	bool mIsLoaded;
	std::string mName;
	std::vector<PhysicsObject*> mColliders;
	Vec2 mBSpawn;
	Vec2 mCSpawn;
};
