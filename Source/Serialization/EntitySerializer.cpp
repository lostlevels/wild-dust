#include "Precompiled.h"
#include "EntitySerializer.h"
#include "Core/Entity.h"
#include "Game/EntityFactory.h"
#include "Game/GameContext.h"

void EntitySerializer::serializeTransformSnapshot(const TransformSnapshot &snapshot, BitStream &stream) {
	auto &position = snapshot.position;

	stream.writeFloat(snapshot.time);
	stream.writeFloat(position.x);
	stream.writeFloat(position.y);
	stream.writeFloat(position.z);
	stream.writeFloat(snapshot.rotation);
	stream.writeU32(snapshot.keys);
}

TransformSnapshot EntitySerializer::deserializeTransformSnapshot(const BitStream &stream) {
	TransformSnapshot snapshot;

	snapshot.time = stream.readFloat();
	snapshot.position.x = stream.readFloat();
	snapshot.position.y = stream.readFloat();
	snapshot.position.z = stream.readFloat();
	snapshot.rotation = stream.readFloat();
	snapshot.keys = stream.readU32();

	return snapshot;
}

void EntitySerializer::serializeCommandSnapshot(const CommandSnapshot &snapshot, BitStream &stream) {
	stream.writeFloat(snapshot.time);
	stream.writeString(snapshot.command);
	stream.writeString(snapshot.owner);
	stream.writeString(snapshot.receiver);

	stream.writeFloat(snapshot.position.x);
	stream.writeFloat(snapshot.position.y);
	stream.writeFloat(snapshot.position.z);
	
	stream.writeU32(snapshot.flags);
}

CommandSnapshot EntitySerializer::deserializeCommandSnapshot(const BitStream &stream) {
	CommandSnapshot snapshot;

	snapshot.time = stream.readFloat();
	snapshot.command = stream.readString();
	snapshot.owner = stream.readString();
	snapshot.receiver = stream.readString();

	snapshot.position.x = stream.readFloat();
	snapshot.position.y = stream.readFloat();
	snapshot.position.z = stream.readFloat();

	snapshot.flags = stream.readU32();
	return snapshot;
}

void EntitySerializer::serializePlayerState(const PlayerState &state, BitStream &stream) {
	stream.writeString(state.name);
	stream.writeString(state.type);

	stream.writeFloat(state.ping);
	stream.writeFloat(state.health);

	stream.writeU32(state.kills);
	stream.writeU32(state.deaths);
	stream.writeU32(state.team);
}

PlayerState EntitySerializer::deserializePlayerState(const BitStream &stream) {
	PlayerState state;

	state.name = stream.readString();
	state.type = stream.readString();

	state.ping = stream.readFloat();
	state.health = stream.readFloat();

	state.kills = stream.readU32();
	state.deaths = stream.readU32();
	state.team = stream.readU32();

	return state;
}

void EntitySerializer::serializeExistingEntity(const Entity &ent, BitStream &stream) {
	auto &transformSnapshots = ent.getTransformSnapshots();
	auto &commandSnapshots = ent.getCommandSnapshots();

	stream.writeString(ent.getId());
	stream.writeString(ent.getType());
	stream.writeString(ent.getOwner());
	stream.writeU32(ent.getSendMode());

	stream.writeU32(transformSnapshots.size());
	for (auto &snapshot : transformSnapshots) {
		EntitySerializer::serializeTransformSnapshot(snapshot, stream);
	}

	stream.writeU32(commandSnapshots.size());
	for (auto &snapshot : commandSnapshots) {
		EntitySerializer::serializeCommandSnapshot(snapshot, stream);
	}
}

void EntitySerializer::serializeEntities(const std::vector<Entity*> &entities, BitStream &stream) {
	stream.writeU32(entities.size());
	for (auto e : entities) {
		// So World can do a lookup, writes id twice -
		// once before serializing the entity and once during the entity serialization
		stream.writeString(e->getId());
		serializeExistingEntity(*e, stream);
	}
}

void EntitySerializer::serializeInitialEntity(float time, const Entity &ent, BitStream &stream) {
	stream.writeString(ent.getId());
	stream.writeString(ent.getType());
	stream.writeString(ent.getOwner());
	stream.writeU32(ent.getSendMode());
	stream.writeU32(1); // 1 transform snapshot no command snapshots

	TransformSnapshot snapshot = {
		time,
		ent.getPosition(),
		0
	};
	EntitySerializer::serializeTransformSnapshot(snapshot, stream);
}

void EntitySerializer::deserializeIntoEntity(Entity *entity, const BitStream &stream) {
	stream.readString(); // id
	stream.readString(); // type
	stream.readString(); // owner
	entity->setSendMode(stream.readU32());

	uint32_t numTransformSnapshots = stream.readU32();
	for (uint32_t i = 0; i < numTransformSnapshots; ++i) {
		entity->addSnapshot(EntitySerializer::deserializeTransformSnapshot(stream));
	}

	uint32_t numCommandSnapshots = stream.readU32();
	for (uint32_t i = 0; i < numCommandSnapshots; ++i) {
		auto snap = EntitySerializer::deserializeCommandSnapshot(stream);
		entity->addSnapshot(snap);
	}
}

Entity *EntitySerializer::deserializeInitialEntity(GameContext *context, const BitStream &stream) {
	std::string name = stream.readString();
	std::string type = stream.readString();
	std::string owner = stream.readString();

	uint32_t sendMode = stream.readU32();
	uint32_t numSnapshots = stream.readU32();
	assert(numSnapshots == 1);

	TransformSnapshot snapshot = EntitySerializer::deserializeTransformSnapshot(stream);

	Entity *e = EntityFactory::createLocalEntity(context, name, type, owner);
	e->setPosition(snapshot.position);
	e->setSendMode(sendMode);

	return e;
}
