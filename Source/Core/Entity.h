#pragma once

#include "Core/BitStream.h"
#include "Core/TransformSnapshot.h"
#include "Core/EntityRepresentation.h"

#include <string>
#include <vector>

enum {
	SEND_ALWAYS = 0,
	SEND_ENTER,
	SEND_NEVER
};

class Entity {
public:
	Entity();
	Entity(const std::string &id, const std::string &type, const std::string &owner, int sendMode);
	Entity(const Entity &other);
	Entity(const std::string &id, const std::string &type, const std::string &owner, int sendMode, const EntityRepresentation &representation);
	virtual ~Entity();

	virtual void update(float dt);

	const std::string &getId() const { return mId; }
	void setId(const std::string &id) { mId = id; }

	const std::string &getOwner() const { return mOwner; }
	void setOwner(const std::string &owner) { mOwner = owner; }

	const std::string &getType() const { return mType; }
	void setType(const std::string &type) { mType = type; }

	void setPosition(const glm::vec3 &position) { mPosition = position; }
	const glm::vec3 &getPosition() const { return mPosition; }
	void interpolate(float time);

	const std::vector<TransformSnapshot> &getSnapshots() const { return mSnapshots; }
	void addSnapshots(const std::vector<TransformSnapshot> &snapshots);
	void queueSnapshot(float time);
	void addSnapshot(const TransformSnapshot &snapshot) { mSnapshots.push_back(snapshot); }
	void removeOldSnapshots();
	void removeAllSnapshots() { mSnapshots.clear(); }

	void setRepresentation(const EntityRepresentation &rep) { mRepresentation = rep; }
	const EntityRepresentation &getRepresentation() const { return mRepresentation; }

	void setSendMode(int mode) { mSendMode = mode; }
	int getSendMode() const { return mSendMode; }

private:
	std::string mId;
	std::string mType;
	std::string mOwner;

	int  mSendMode;
	Vec3 mPosition;
	std::vector<TransformSnapshot> mSnapshots;
	EntityRepresentation mRepresentation;

	TransformSnapshot getSnapshot(float time);
};
