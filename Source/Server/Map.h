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

private:
	void createCollider(int x, int y, int w, int h);
	void unload();

private:
	Server *mServer;
	bool mIsLoaded;
	std::string mName;
	std::vector<PhysicsObject*> mColliders;
};
