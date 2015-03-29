#pragma once

#include "Core/World.h"
#include "Core/Bitstream.h"
#include "Core/PlayerState.h"
#include "Core/CommandSnapshot.h"
#include "Network/Connection.h"

class SERVER_API ServerWorld : public World {
public:
	ServerWorld();
	virtual ~ServerWorld();

	void update(float dt);
	void serve(unsigned short ip);
private:
	// Each connected player is in here - not each entity controlled by player (which can include bullets)
	// Client NEVER sends PlayerState to server, only server sends to clients
	std::map<std::string, PlayerState> mPlayerStates;
	BitStream                     *mStream;
	Connection                    mConn;

	float                         mStateSendTimer;
	std::vector<Entity*>          mEntsToSend;

	std::string                   mLastCharacter;

	float                         mGameStateSendTimer;
	PlayerState *getPlayerState(const std::string &id);

	void onClientEntered(const std::string &playerName);
	void onPlayerUpdate(const BitStream &stream);

	void handleStateUpdates(float dt);
	void sendGameState();

	void sendDeathMessage(const std::string &dyingEntId);
	void sendGameMessage(const char *format, ...);
	void sendPlayerLeft(const std::string &id);
	void handleSendGameStates(float dt);

	void processCommand(const CommandSnapshot &snapshot);

	virtual void update(float gameTime, float dt);
};
