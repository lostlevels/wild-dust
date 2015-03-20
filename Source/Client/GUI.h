#pragma once

class Client;
class Font;

class GUI {
public:
	GUI(Client *client);
	~GUI();

	bool init();
	void shutdown();

	void render();

private:
	Client *mClient;
	Font *mVera;
};