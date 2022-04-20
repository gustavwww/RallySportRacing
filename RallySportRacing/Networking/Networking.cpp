#include <thread>
#include <iostream>
#include <map>
#include <glm/glm.hpp>

#include "Networking.h"
#include "Services/TCPClient.h"
#include "Services/UDPClient.h"
#include "Services/Protocol/Command.h"
#include "Services/Protocol/ProtocolParser.h"
#include "Game/GameObject.h"
#include "Game/Vehicle.h"
#include "Player.h"
#include "Rendering/SDLWindowHandler.h"
#include "Rendering/Model.h"
#include <glm/gtx/quaternion.hpp>

#define TICK_RATE 120;

using namespace std;

namespace Networking {

	Server::TCPClient tcpClient;
	Server::UDPClient udpClient;
	thread tcpThread;
	thread udpThread;
	thread sendThread;

	Game::Vehicle* vehicle;
	Rendering::SDLWindowHandler* handler;

	bool inGame = false;

	int clientID;

	map<int, Player*> players;

	void setupNetwork(Game::Vehicle* playerObj, Rendering::SDLWindowHandler* windowHandler) {
		vehicle = playerObj;
		handler = windowHandler;

		tcpClient.addCallback(Networking::tcpPacketReceived);
		tcpClient.connectToServer();

		udpClient.addCallback(Networking::udpPacketReceived);

		tcpThread = thread(&Server::TCPClient::listen, tcpClient);
		udpThread = thread(&Server::UDPClient::listen, udpClient);
	}

	void joinGame(string id, string name) {
		tcpClient.sendPacket("join:" + id + "," + name);
	}

	void tcpPacketReceived(string str) {
		Protocol::Command cmd = Protocol::parseMessage(str);
		string command = cmd.getCommand();

		if (command == "connect") {
			clientID = stoi(cmd.getArgs()[0]);
			cout << "Client ID received from server: " << clientID << endl;
			joinGame("hub", "Gustav");

		} else if (command == "error") {
			cout << "Server Error: " << cmd.getArgs()[0] << endl;

		} else if (command == "success") {
			if (cmd.getArgs()[0] == "joined") {
				cout << "Successfully joined game." << endl;
				inGame = true;
				sendThread = thread(&Networking::sendStatusPacket);
			}

		}

	}

	void udpPacketReceived(string str) {
		Protocol::Command cmd = Protocol::parseMessage(str);
		string command = cmd.getCommand();

		if (command == "game") {

			// Store players already in game to eventuelly remove them.
			vector<int> playersInGame;
			for (auto el : players)
				playersInGame.push_back(el.first);

			for (int i = 1; i < cmd.getArgsSize(); i += 10) {
				int id = stoi(cmd.getArgs()[i]);
				if (id == clientID) {
					continue;
				}

				PlayerData data(cmd, i);

				// TODO: Spawn model if player not already spawned.
				auto el = players.find(id);
				if (el == players.end()) {
					// Player not initialized, creating player...
					
					Player* p = new Player(handler, data);
					players.insert(pair<int, Player*>(id, p));
					cout << "A player has joined the game: " << p->getName() << endl;
				}
				else {
					// Player already created, updating position...
					Player* p = el->second;
					p->updateState(data);
					playersInGame.erase(find(playersInGame.begin(), playersInGame.end(), id));
				}

			}

			// Remove players no longer in game.
			for (int id : playersInGame) {
				auto el = players.find(id);
				if (el != players.end()) {
					Player* p = el->second;
					handler->removeModel(p->getModel());
					players.erase(el);
					cout << "A player has left the game: " << p->getName() << endl;
					// TODO: Fix player deletion, currently kills the game..
					//delete p;
				}
			}

		}

	}

	void sendStatusPacket() {
		while (inGame) {
			glm::vec3 pos = vehicle->getPosition();
			glm::quat qu = vehicle->getQuaternion();
			udpClient.sendPacket(to_string(clientID) + "-" + "pos:" + to_string(pos.x) + ", "
				+ to_string(pos.y) + ","
				+ to_string(pos.z) + ","
				+ to_string(qu.x) + ","
				+ to_string(qu.y) + ","
				+ to_string(qu.z) + ","
				+ to_string(qu.w));
			int rate = 1000 / TICK_RATE;
			this_thread::sleep_for(chrono::milliseconds(rate));
		}
	}

	void terminateNetwork() {
		cout << "Terminating network..." << endl;
		inGame = false;
		sendThread.join();
		//udpClient.terminate();
		//tcpClient.terminate();
	}


}