#include "Precompiled.h"
#include "GUI.h"
#include "Font.h"
#include "Renderer.h"

GUI::GUI(Renderer *renderer) {
	mRenderer = renderer;
}

GUI::~GUI() {

}

bool GUI::init() {
	mVera = new Font(mRenderer);
	mVera->loadFromFile("../Content/Fonts/Vera.ttf", 22);
	return true;
}

void GUI::shutdown() {
	delete mVera;
}

void GUI::render() {
	render(mData);
}

void GUI::render(const std::vector<GUIData> &datas) {
	for (auto &data : datas) {
		mVera->drawText(data.text, data.x, data.y);
	}
	// ClientNetworkState state = mClient->getNetworkState();
	// const char *status;
	// switch (state) {
	// case CLIENT_CONNECTING:
	// 	status = "Connecting..";
	// 	break;

	// case CLIENT_CONNECTED:
	// 	status = "Connected.";
	// 	break;

	// case CLIENT_IDLE:
	// 	status = "Idle..";
	// 	break;
	// }
	// mVera->drawText(status, 25, 25);
}