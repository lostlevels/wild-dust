#include "Precompiled.h"
#include "TilemapEntity.h"

TilemapEntity::TilemapEntity(const std::string &id, const std::string &filename) : Entity(id) {
	loadTmx(filename);
}

TilemapEntity::~TilemapEntity() {

}

void TilemapEntity::loadTmx(const std::string &filename) {
	mMap.ParseFile(filename);
	assert(!mMap.HasError());
}
