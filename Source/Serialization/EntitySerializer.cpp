#include "Precompiled.h"
#include "EntitySerializer.h"
#include "Core/Entity.h"
#include "Core/EntityFactory.h"

void EntitySerializer::serializeSnapshot(const TransformSnapshot &snapshot, BitStream &stream) {
	auto &position = snapshot.position;

	stream.writeFloat(snapshot.time);
	stream.writeFloat(position.x);
	stream.writeFloat(position.y);
	stream.writeFloat(position.z);
	stream.writeFloat(snapshot.rotation);
}

TransformSnapshot EntitySerializer::deserializeSnapshot(const BitStream &stream) {
	TransformSnapshot snapshot;

	snapshot.time = stream.readFloat();
	snapshot.position.x = stream.readFloat();
	snapshot.position.y = stream.readFloat();
	snapshot.position.z = stream.readFloat();
	snapshot.rotation = stream.readFloat();

	return snapshot;
}

void EntitySerializer::serializePlayerState(const PlayerState &state, BitStream &stream) {
	stream.writeString(state.name);
	stream.writeString(state.type);

	stream.writeFloat(state.ping);

	stream.writeU32(state.kills);
	stream.writeU32(state.deaths);
	stream.writeU32(state.team);
}

PlayerState EntitySerializer::deserializePlayerState(const BitStream &stream) {
	PlayerState state;

	state.name = stream.readString();
	state.type = stream.readString();

	state.ping = stream.readFloat();

	state.kills = stream.readU32();
	state.deaths = stream.readU32();
	state.team = stream.readU32();

	return state;
}

void EntitySerializer::serializeExistingEntity(const Entity &ent, BitStream &stream) {
	auto &snapshots = ent.getSnapshots();

	stream.writeString(ent.getId());
	stream.writeString(ent.getType());
	stream.writeString(ent.getOwner());
	stream.writeU32(ent.getSendMode());
	stream.writeU32(snapshots.size());

	for (auto &snapshot : snapshots) {
		EntitySerializer::serializeSnapshot(snapshot, stream);
	}
}

void EntitySerializer::serializeEntities(const std::vector<Entity*> &entities, BitStream &stream) {
	stream.writeU32(entities.size());
	for (auto e : entities) {
		// So World can do a lookup
		stream.writeString(e->getId());
		serializeExistingEntity(*e, stream);
	}
}

void EntitySerializer::serializeInitialEntity(float time, const Entity &ent, BitStream &stream) {
	stream.writeString(ent.getId());
	stream.writeString(ent.getType());
	stream.writeString(ent.getOwner());
	stream.writeU32(ent.getSendMode());
	stream.writeU32(1); // 1 snapshot

	TransformSnapshot snapshot = {
		time,
		ent.getPosition(),
		0
	};
	EntitySerializer::serializeSnapshot(snapshot, stream);
}

void EntitySerializer::deserializeIntoEntity(Entity *entity, const BitStream &stream) {
	stream.readString(); // id
	stream.readString(); // type
	stream.readString(); // owner
	entity->setSendMode(stream.readU32());

	uint32_t numPackets = stream.readU32();
	for (uint32_t i = 0; i < numPackets; ++i) {
		entity->addSnapshot(EntitySerializer::deserializeSnapshot(stream));
	}
}

Entity *EntitySerializer::deserializeInitialEntity(const BitStream &stream) {
	std::string name = stream.readString();
	std::string type = stream.readString();
	std::string owner = stream.readString();

	uint32_t sendMode = stream.readU32();
	uint32_t numSnapshots = stream.readU32();

	TransformSnapshot snapshot = EntitySerializer::deserializeSnapshot(stream);

	Entity *e = EntityFactory::createEntity(name, type, owner);
	e->setPosition(snapshot.position);
	e->setSendMode(sendMode);

	return e;
}
