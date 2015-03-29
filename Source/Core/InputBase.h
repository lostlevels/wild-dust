#pragma once

enum {
	BTN_MOVE_LEFT = (1<<0),
	BTN_MOVE_RIGHT = (1<<1),
	BTN_JUMP = (1<<2),
	BTN_ATTACK = (1<<3),
	BTN_INFO = (1<<4)
};

class InputBase {
public:
	virtual int32_t getButtons() const = 0;
	virtual int32_t getNewButtons() const = 0;
};