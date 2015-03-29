#pragma once

#include "Core/BitStream.h"
#include "Core/TransformSnapshot.h"
#include "Core/CommandSnapshot.h"
#include "Core/PlayerState.h"

class Entity;
class GameContext;

class EntitySerializer {
public:
	static void serializeExistingEntity(const Entity &ent, BitStream &stream);
	static void deserializeIntoEntity(Entity *entity, const BitStream &stream);

	static void serializeTransformSnapshot(const TransformSnapshot &snapshot, BitStream &stream);
	static TransformSnapshot deserializeTransformSnapshot(const BitStream &stream);
	static void serializeCommandSnapshot(const CommandSnapshot &snapshot, BitStream &stream);
	static CommandSnapshot deserializeCommandSnapshot(const BitStream &stream);

	static void serializePlayerState(const PlayerState &playerState, BitStream &stream);
	static PlayerState deserializePlayerState(const BitStream &stream);

	static void serializeInitialEntity(float time, const Entity &ent, BitStream &stream);
	static Entity *deserializeInitialEntity(GameContext *context, const BitStream &stream);

	static void serializeEntities(const std::vector<Entity*> &entities, BitStream &stream);
};
