#pragma once

#include "Entity.h"
#include <Tmx.h>

class CORE_API TilemapEntity : public Entity {
public:
	TilemapEntity(const std::string &id, const std::string &filename);
	virtual ~TilemapEntity();

	virtual std::string getRenderType() const { return "tilemap"; }

	Tmx::Map &getMap() { return mMap; }
private:
	Tmx::Map mMap;

	void loadTmx(const std::string &filename);
};
