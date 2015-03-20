#pragma once

#include "Shared/Protocol.h"

class Client;

class InputSystem {
public:
	InputSystem(Client *client);
	
	PlayerInput makeInputState();

private:
	Client *mClient;
};
