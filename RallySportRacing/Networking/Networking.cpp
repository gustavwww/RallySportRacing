#include <thread>
#include <iostream>

#include "Networking.h"
#include "Services/TCPClient.h"
#include "Services/Protocol/Command.h"
#include "Services/Protocol/ProtocolParser.h"
#include "Game/GameObject.h"

using namespace std;

namespace Networking {

	Server::TCPClient tcpClient;
	thread tcpThread;

	Game::GameObject* playerObj;

	void setupNetwork() {

		tcpClient = Server::TCPClient();
		tcpClient.addCallback(Networking::tcpPacketReceived);
		tcpClient.connectToServer();

		tcpThread = thread(&Server::TCPClient::listen, tcpClient);
	}

	void joinGame(string id, string name) {
		tcpClient.sendPacket("join:" + id + "," + name);
	}

	void tcpPacketReceived(string str) {
		Protocol::Command cmd = Protocol::parseMessage(str);
		string command = cmd.getCommand();

		if (command == "game") {
			for (int i = 0; i < cmd.getArgsSize(); i += 4) {
				string name = cmd.getArgs()[i + 1];
				float x = stof(cmd.getArgs()[i + 2]);
				float y = stof(cmd.getArgs()[i + 3]);
				float z = stof(cmd.getArgs()[i + 4]);

				// TODO: Spawn model if player not already spawned.
			}

		} else if (command == "error") {
			cout << "Error: " << cmd.getArgs()[0] << endl;
		}

	}

	void sendPosition(glm::vec3 position) {
		tcpClient.sendPacket("pos:" + to_string(position.x) + ","
			+ to_string(position.y) + ","
			+ to_string(position.z));
	}


}