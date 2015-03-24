#pragma once

#include "Core/World.h"
#include "Core/BitStream.h"
#include "Core/Clock.h"
#include "Network/Connection.h"
#include "Core/PlayerState.h"

class InputSystem;
class AudioSystem;

class ClientWorld : public World {
public:
	ClientWorld(InputSystem *input, AudioSystem *audioSystem);
	virtual ~ClientWorld();

	void connect(const std::string &hostName, unsigned short port);

	virtual void update(float dt);

private:
	std::map<std::string, PlayerState> mPlayerStates;

	Connection     mConn;
	BitStream      *mStream;

	InputSystem    *mInput;
	AudioSystem    *mAudio;

	float           mSnapshotTimer;
	float           mSendTimer;

	Entity *getMe();
	void handlePlayerInput(float dt);
	void sendQueuedPackets(float dt);

	float getPing(const std::string &name) const;
};
