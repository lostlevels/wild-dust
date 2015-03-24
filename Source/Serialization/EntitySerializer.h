#pragma once

#include "Core/BitStream.h"
#include "Core/TransformSnapshot.h"
#include "Core/PlayerState.h"

class Entity;

class EntitySerializer {
public:
	static void serializeExistingEntity(const Entity &ent, BitStream &stream);
	static void deserializeIntoEntity(Entity *entity, const BitStream &stream);

	static void serializeSnapshot(const TransformSnapshot &snapshot, BitStream &stream);
	static TransformSnapshot deserializeSnapshot(const BitStream &stream);

	static void serializePlayerState(const PlayerState &playerState, BitStream &stream);
	static PlayerState deserializePlayerState(const BitStream &stream);

	static void serializeInitialEntity(float time, const Entity &ent, BitStream &stream);
	static Entity *deserializeInitialEntity(const BitStream &stream);

	static void serializeEntities(const std::vector<Entity*> &entities, BitStream &stream);
};
