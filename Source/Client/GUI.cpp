#include "Precompiled.h"
#include "GUI.h"
#include "Font.h"
#include "Client.h"

GUI::GUI(Client *client) {
	mClient = client;
}

GUI::~GUI() {

}

bool GUI::init() {
	mVera = new Font(mClient->getRenderer());
	mVera->loadFromFile("../Content/Fonts/Vera.ttf", 22);
	return true;
}

void GUI::shutdown() {
	delete mVera;
}

void GUI::render() {
	ClientNetworkState state = mClient->getNetworkState();
	const char *status;
	switch (state) {
	case CLIENT_CONNECTING:
		status = "Connecting..";
		break;

	case CLIENT_CONNECTED:
		status = "Connected.";
		break;

	case CLIENT_IDLE:
		status = "Idle..";
		break;

	case CLIENT_INGAME:
		status = "In-game.";
		break;
	}
	mVera->drawText(status, 25, 25);
}
