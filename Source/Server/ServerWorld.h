#pragma once

#include "Core/World.h"
#include "Core/Bitstream.h"
#include "Core/PlayerState.h"
#include "Network/Connection.h"

class ServerWorld : public World {
public:
	ServerWorld();
	virtual ~ServerWorld();

	virtual void update(float dt);

	void serve(unsigned short ip);
private:
	// Each connected player is in here - not each entity controlled by player (which can include bullets)
	// Client NEVER sends PlayerState to server, only server sends to clients
	std::map<std::string, PlayerState> mPlayerStates;
	BitStream                     *mStream;
	Connection                    mConn;

	float                         mStateSendTimer;
	std::vector<Entity*>          mEntsToSend;

	void onClientEntered(const std::string &playerName);

	void handleStateUpdates(float dt);
	void sendGameState();
};
