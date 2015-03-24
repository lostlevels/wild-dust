#include "Precompiled.h"
#include "Map.h"
#include "Server.h"
#include <Tmx.h>
#include "Physics/Physics.h"
#include "Physics/PhysicsObject.h"

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

void ServerMap::unload() {
	for (PhysicsObject *collider : mColliders) {
		delete collider;
	}
	mColliders.clear();

	mIsLoaded = false;
	mName.clear();
}

void ServerMap::createCollider(int x, int y, int w, int h) {
	Physics *phys = mServer->getPhysics();
	PhysicsObject *object = phys->createObject(PHYSICS_STATIC);
	object->setBox(w, h);
	object->setPosition(Vec2(x, y));
	object->setFriction(150.0f);
	mColliders.push_back(object);
}
