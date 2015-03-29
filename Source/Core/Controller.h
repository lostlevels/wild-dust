#pragma once

class Entity;

class Controller {
public:
	Controller(){}
	virtual ~Controller(){}
	virtual void control(Entity *e, float gameTime, float dt) = 0;
	virtual void onAttached(Entity *e){}

private:
	Controller(const Controller&) = delete;
	Controller& operator = (const Controller&) = delete;
};
