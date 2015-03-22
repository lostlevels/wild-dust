#pragma once

#include "Core/BitStream.h"
#include <string>
#include <vector>
#include "TransformSnapshot.h"

class Entity {
public:
	Entity(const std::string &owner);
	virtual ~Entity();

	virtual void update(float dt);

	const std::string &getOwner() const { return mOwner; }

	void setPosition(const glm::vec3 &position) { mPosition = position; }
	const glm::vec3 &getPosition() const { return mPosition; }
	void interpolate(float time);

	const std::vector<TransformSnapshot> &getSnapshots() const { return mSnapshots; }
	void addSnapshots(const std::vector<TransformSnapshot> &snapshots);

	void removeOldSnapshots();

private:
	// Networked owner, if any
	std::string mOwner;
	std::string mTypeName;

	Vec3 mPosition;
	std::vector<TransformSnapshot> mSnapshots;

	TransformSnapshot getSnapshot(float time);
};
