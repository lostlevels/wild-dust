#pragma once

class Server;

class ServerMap {
public:
	ServerMap(Server *server);
	~ServerMap();

	bool loadFromFile(const std::string &filename);
	bool isLoaded() const { return mIsLoaded; }
	std::string getName() const { return mName; }

private:
	void createCollisionObject(int x, int y, int w, int h);
	void unload();

private:
	Server *mServer;
	bool mIsLoaded;
	std::string mName;
	std::vector<b2Body*> mCollisionObjects;
};
