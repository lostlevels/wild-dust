#pragma once

#include <string>

struct CommandSnapshot {
	float          time;
	std::string    command;
	std::string    owner;
	Vec3           position;
	uint32_t       flags;

	CommandSnapshot(float time = 0, const std::string &command = "", const std::string &owner = "", const Vec3 &position = Vec3(0.0f), uint32_t flags = 0);
};

inline CommandSnapshot::CommandSnapshot(float time, const std::string &command, const std::string &owner, const Vec3 &position, uint32_t flags) :
	time(time),
	command(command),
	owner(owner),
	position(position),
	flags(flags)
{

}
