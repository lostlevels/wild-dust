#pragma once

#include "Shared/ClientCommand.h"

class Client;

class InputSystem {
public:
	InputSystem(Client *client);
	
	PlayerInput makeInputState();

private:
	Client *mClient;
};
