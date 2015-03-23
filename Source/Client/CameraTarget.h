#pragma once

class ICameraTarget {
public:
	virtual ~ICameraTarget() {}
	virtual Vec2 ICameraTarget_getPosition() const = 0;
	virtual Vec2 ICameraTarget_getSize() const = 0;
};
