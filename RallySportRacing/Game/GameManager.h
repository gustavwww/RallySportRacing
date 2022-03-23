#ifndef GAME
#define GAME

#include <string>
#include "GameObject.h"
#include "Rendering/SDLWindowHandler.h"

namespace Game {

	void setupGame(Rendering::SDLWindowHandler* windowHandler);
	void setupNetwork();
	void tcpPacketReceived(string str);
	void update();
	void adjustCamPosition();
}

#endif