#pragma once

enum {
	BTN_MOVE_LEFT = (1<<0),
	BTN_MOVE_RIGHT = (1<<1),
	BTN_JUMP = (1<<2),
	BTN_ATTACK = (1<<3)
};

//
// TODO: Make this base class, and sdl version inherit from
//

class InputSystem {
public:
	InputSystem();
	int32_t getButtons() const { return mButtons; }

	virtual void update(float dt);

private:
	int32_t mButtons;
};
