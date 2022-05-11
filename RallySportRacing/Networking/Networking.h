#ifndef NETWORKING
#define NETWORKING

#include <string>

#include "Game/Vehicle.h"
#include "Rendering/SDLWindowHandler.h"
#include "Networking/PlayerTime.h"

using namespace std;

namespace Networking {
	
	void setupNetwork(Game::Vehicle* playerObj, Rendering::SDLWindowHandler* windowHandler);
	void joinGame(string id, string name);
	void sendTime(float time);
	vector<PlayerTime> getTimes();
	void tcpPacketReceived(string str);
	void udpPacketReceived(string str);
	void sendStatusPacket();
	void terminateNetwork();
}

#endif