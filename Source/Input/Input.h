#pragma once

#include "Core/InputBase.h"

class INPUT_API InputSystem : public InputBase {
public:
	InputSystem();

	virtual int32_t getButtons() const { return mButtons; }
	virtual int32_t getNewButtons() const { return mButtons ^ (mButtons & mLastButtons); }

	virtual void update(float dt);

private:
	int32_t mButtons;
	int32_t mLastButtons;
};
