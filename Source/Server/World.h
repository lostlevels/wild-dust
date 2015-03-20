#pragma once

class SV_Entity;

class ServerWorld {
public:
	ServerWorld();
	~ServerWorld();

	void deleteAllEntities();

	void update(float dt);

private:
	std::vector<SV_Entity*> mEntities;
};