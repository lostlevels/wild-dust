#include "Precompiled.h"
#include "Input.h"

InputSystem::InputSystem() : mButtons(0), mLastButtons(0) {

}

void InputSystem::update(float) {
	mLastButtons = mButtons;
	mButtons = 0;

	const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);

	if (keyboardState[SDL_SCANCODE_A]) {
		mButtons |= BTN_MOVE_LEFT;
	}
	if (keyboardState[SDL_SCANCODE_D]) {
		mButtons |= BTN_MOVE_RIGHT;
	}
	if (keyboardState[SDL_SCANCODE_W]) {
		mButtons |= BTN_JUMP;
	}
	if (keyboardState[SDL_SCANCODE_J]) {
		mButtons |= BTN_ATTACK;
	}
	if (keyboardState[SDL_SCANCODE_TAB]) {
		mButtons |= BTN_INFO;
	}
}
