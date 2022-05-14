#ifndef NETWORKING
#define NETWORKING

#include <string>
#include <map>

#include "Game/Vehicle.h"
#include "Rendering/SDLWindowHandler.h"
#include "Networking/PlayerTime.h"
#include "Networking/Player.h"

using namespace std;

namespace Networking {
	
	void setupNetwork(string playerName, Game::Vehicle* playerObj, int playerColorIndex, Rendering::SDLWindowHandler* windowHandler);
	void joinGame(string id, string name);
	void sendTime(float time);
	void setColor(int index);
	vector<PlayerTime> getTimes();
	map<int, Player*> getPlayers();
	void tcpPacketReceived(string str);
	void udpPacketReceived(string str);
	void sendStatusPacket();
	void terminateNetwork();
}

#endif