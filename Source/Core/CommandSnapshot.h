#pragma once

#include <string>

struct CommandSnapshot {
	float          time;
	std::string    command;
	std::string    owner;
	std::string    receiver;
	Vec3           position;
	uint32_t       flags;

	CommandSnapshot(float time = 0, const std::string &command = "", const std::string &owner = "", const std::string &receiver = "", const Vec3 &position = Vec3(0.0f), uint32_t flags = 0);
};

inline CommandSnapshot::CommandSnapshot(float time, const std::string &command, const std::string &owner, const std::string &receiver,
 const Vec3 &position, uint32_t flags) :
	time(time),
	command(command),
	owner(owner),
	receiver(receiver),
	position(position),
	flags(flags)
{

}
