#pragma once

class Client;
class CL_Entity;

class ClientWorld {
public:
	ClientWorld(Client *client);
	~ClientWorld();

	void deleteAllEntities();

	void update(float dt);

private:
	Client *mClient;
	std::vector<CL_Entity*> mEntities;
};