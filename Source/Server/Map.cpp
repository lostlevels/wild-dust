#include "Precompiled.h"
#include "Map.h"
#include "Server.h"
#include <Tmx.h>

ServerMap::ServerMap(Server *server) {
	mServer = server;
	mIsLoaded = false;
}

ServerMap::~ServerMap() {
	unload();
}

bool ServerMap::loadFromFile(const std::string &mapName) {
	Tmx::Map tmxMap;
	tmxMap.ParseFile(std::string("../Content/Maps/") + mapName);

	if (tmxMap.HasError()) {
		gLogger.error("Could not load map '%s`: %s\n", mapName.c_str(), tmxMap.GetErrorText());
		return false;
	}

	unload();

	for (int i = 0; i < tmxMap.GetNumObjectGroups(); ++i) {
		const Tmx::ObjectGroup *tmxGroup = tmxMap.GetObjectGroup(i);
		for (int j = 0; j < tmxGroup->GetNumObjects(); ++j) {
			const Tmx::Object *tmxObject = tmxGroup->GetObject(j);
			createCollisionObject(
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

void ServerMap::unload() {
	b2World *physWorld = mServer->getPhysicsWorld();

	for (b2Body *object : mCollisionObjects) {
		physWorld->DestroyBody(object);
	}
	mCollisionObjects.clear();

	mIsLoaded = false;
	mName.clear();
}

void ServerMap::createCollisionObject(int x, int y, int w, int h) {
	b2World *physWorld = mServer->getPhysicsWorld();


	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.fixedRotation = true;

	int realX = x + w / 2;
	int realY = y + h / 2;

	b2Body *object = physWorld->CreateBody(&bodyDef);
	object->SetTransform(b2Vec2(realX, realY), 0.0f);

	b2PolygonShape shape;
	shape.SetAsBox(w / 2, h / 2);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 100.0f;

	object->CreateFixture(&fixtureDef);

	mCollisionObjects.push_back(object);
}
