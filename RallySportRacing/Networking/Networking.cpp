#include <thread>
#include <iostream>
#include <map>
#include <glm/glm.hpp>

#include "Networking.h"
#include "Services/TCPClient.h"
#include "Services/Protocol/Command.h"
#include "Services/Protocol/ProtocolParser.h"
#include "Game/GameObject.h"
#include "Player.h"
#include "Rendering/SDLWindowHandler.h"
#include "Rendering/Model.h"

using namespace std;

namespace Networking {

	Server::TCPClient tcpClient;
	thread tcpThread;
	thread sendThread;

	Game::GameObject* obj;
	Rendering::SDLWindowHandler* handler;

	int clientID;

	bool inGame = false;

	map<int, Player*> players;

	void setupNetwork(Game::GameObject* playerObj, Rendering::SDLWindowHandler* windowHandler) {
		obj = playerObj;
		handler = windowHandler;

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

		if (command == "connect") {
			clientID = stoi(cmd.getArgs()[0]);
			cout << "Client ID received from server: " << clientID << endl;
			joinGame("hub", "Gustav");

		} else if (command == "game") {
			for (int i = 0; i < cmd.getArgsSize(); i += 6) {
				int id = stoi(cmd.getArgs()[i + 1]);
				if (id == clientID) {
					continue;
				}

				string name = cmd.getArgs()[i + 2];
				float x = stof(cmd.getArgs()[i + 3]);
				float y = stof(cmd.getArgs()[i + 4]);
				float z = stof(cmd.getArgs()[i + 5]);

				// TODO: Spawn model if player not already spawned.
				auto el = players.find(id);
				if (el == players.end()) {
					// Player not initialized, creating player...
					cout << "A player has joined the game: " << name << endl;
					Rendering::Model* model = Rendering::Model::loadModel("../Models/PorscheGT3_wWheels.gltf");
					handler->addModel(model);
					Game::GameObject* obj = new Game::GameObject(model);
					obj->setPosition(glm::vec3(x, y, z));

					Player* p = new Player(name, obj);
					players.insert(pair<int, Player*>(id, p));
				} else {
					// Player already created, updating position...
					Player* p = el->second;
					p->setPosition(glm::vec3(x,y,z));
				}

			}

		} else if (command == "error") {
			cout << "Server Error: " << cmd.getArgs()[0] << endl;

		} else if (command == "success") {
			if (cmd.getArgs()[0] == "joined") {
				// Begin sending positions.
				cout << "Successfully joined game." << endl;
				inGame = true;
				sendThread = thread(&Networking::sendStatusPacket);
			}

		}

	}

	void sendStatusPacket() {
		while (inGame) {
			glm::vec3 pos = obj->getPosition();
			tcpClient.sendPacket("pos:" + to_string(pos.x) + ","
				+ to_string(pos.y) + ","
				+ to_string(pos.z));
			this_thread::sleep_for(2ms);
		}
	}


}