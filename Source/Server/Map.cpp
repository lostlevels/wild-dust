#include "Precompiled.h"
#include "Map.h"
#include "Server.h"
#include <Tmx.h>
#include "Physics/Physics.h"
#include "Physics/PhysicsObject.h"
#include "World.h"
#include "SV_Prop.h"

#ifndef stricmp
#define stricmp strcmp
#endif

ServerMap::ServerMap(Server *server) {
	mServer = server;
	mIsLoaded = false;
	mCSpawn = Vec2(0.0f, 0.0f);
	mBSpawn = Vec2(0.0f, 0.0f);
}

ServerMap::~ServerMap() {
	unload();
}

bool ServerMap::loadFromFile(const std::string &mapName) {
	Tmx::Map tmxMap;
	tmxMap.ParseFile(std::string("../Content/Maps/") + mapName);

	if (tmxMap.HasError()) {
		gLogger.error("Could not load map '%s`: %s\n", mapName.c_str(), tmxMap.GetErrorText().c_str());
		return false;
	}

	unload();

	ServerWorld *world = mServer->getWorld();

	for (int i = 0; i < tmxMap.GetNumObjectGroups(); ++i) {
		const Tmx::ObjectGroup *tmxGroup = tmxMap.GetObjectGroup(i);
		if (stricmp(tmxGroup->GetName().c_str(), "Collision") == 0) {
			for (int j = 0; j < tmxGroup->GetNumObjects(); ++j) {
				const Tmx::Object *tmxObject = tmxGroup->GetObject(j);
				createCollider(
					tmxObject->GetX(),
					tmxObject->GetY(),
					tmxObject->GetWidth(),
					tmxObject->GetHeight());
			}
		}
		else if (stricmp(tmxGroup->GetName().c_str(), "Entities") == 0) {
			for (int j = 0; j < tmxGroup->GetNumObjects(); ++j) {
				const Tmx::Object *tmxObject = tmxGroup->GetObject(j);
				if (stricmp(tmxObject->GetType().c_str(), "Prop") == 0) {
					SV_Prop *prop = world->spawnEntityTyped<SV_Prop>(ENTITY_PROP);
					prop->mSpriteName = tmxObject->GetProperties().GetStringProperty("Sprite");
					prop->mPosition.x = tmxObject->GetX();
					prop->mPosition.y = tmxObject->GetY();
					prop->mSize.x = tmxObject->GetWidth();
					prop->mSize.y = tmxObject->GetHeight();
				}
				else if (stricmp(tmxObject->GetType().c_str(), "Spawn") == 0) {
					std::string team = tmxObject->GetProperties().GetStringProperty("Team");
					if (!stricmp(team.c_str(), "Bandits")) {
						mBSpawn.x = tmxObject->GetX();
						mBSpawn.y = tmxObject->GetY();
					}
					else {
						mCSpawn.x = tmxObject->GetX();
						mCSpawn.y = tmxObject->GetY();
					}
				}
			}
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
