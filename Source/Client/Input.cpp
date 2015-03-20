#include "Precompiled.h"
#include "Input.h"
#include "Client.h"

InputSystem::InputSystem(Client *client) {
	mClient = client;
}

PlayerInput InputSystem::makeInputState() {
	PlayerInput state;
	state.buttonMask = 0;

	int numkeys;
	const Uint8 *keyboardState = SDL_GetKeyboardState(&numkeys);

	if (keyboardState[SDL_SCANCODE_LEFT]) {
		state.buttonMask |= BTN_MOVE_LEFT;
	}
	if (keyboardState[SDL_SCANCODE_RIGHT]) {
		state.buttonMask |= BTN_MOVE_RIGHT;
	}
	if (keyboardState[SDL_SCANCODE_SPACE]) {
		state.buttonMask |= BTN_JUMP;
	}
	if (keyboardState[SDL_SCANCODE_X]) {
		state.buttonMask |= BTN_ATTACK;
	}

	return state;
}
