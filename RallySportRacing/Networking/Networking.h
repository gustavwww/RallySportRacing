#ifndef NETWORKING
#define NETWORKING

#include <string>

#include "Game/GameObject.h"
#include "Rendering/SDLWindowHandler.h"

using namespace std;

namespace Networking {

	void setupNetwork(Game::GameObject* playerObj, Rendering::SDLWindowHandler* windowHandler);
	void joinGame(string id, string name);
	void tcpPacketReceived(string str);
	void sendStatusPacket();
}

#endif