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
#include "Player.h"
#include "Rendering/SDLWindowHandler.h"
#include "Rendering/Model.h"
#include <glm/gtx/quaternion.hpp>

using namespace std;

namespace Networking {

	Server::TCPClient tcpClient;
	Server::UDPClient udpClient;
	thread tcpThread;
	thread udpThread;
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

			for (int i = 1; i < cmd.getArgsSize(); i += 10) {
				int id = stoi(cmd.getArgs()[i]);
				if (id == clientID) {
					continue;
				}

				string name = cmd.getArgs()[i + 1];
				float posX = stof(cmd.getArgs()[i + 2]);
				float posY = stof(cmd.getArgs()[i + 3]);
				float posZ = stof(cmd.getArgs()[i + 4]);
				float quX = stof(cmd.getArgs()[i + 5]);
				float quY = stof(cmd.getArgs()[i + 6]);
				float quZ = stof(cmd.getArgs()[i + 7]);
				float quW = stof(cmd.getArgs()[i + 8]);

				// TODO: Spawn model if player not already spawned.
				auto el = players.find(id);
				if (el == players.end()) {
					// Player not initialized, creating player...
					cout << "A player has joined the game: " << name << endl;
					Rendering::Model* model = Rendering::Model::loadModel("../Models/PorscheGT3_wWheels.gltf", false);
					handler->addModel(model);
					Game::GameObject* obj = new Game::GameObject(model);
					obj->setPosition(glm::vec3(posX, posY, posZ));
					obj->setQuaternion(glm::quat(quW, quX, quY, quZ));

					Player* p = new Player(name, obj);
					players.insert(pair<int, Player*>(id, p));
				}
				else {
					// Player already created, updating position...
					Player* p = el->second;
					p->setPosition(glm::vec3(posX, posY, posZ));
					p->setQuaternion(glm::quat(quW, quX, quY, quZ));
				}

			}

		}

	}

	void sendStatusPacket() {
		while (inGame) {
			glm::vec3 pos = obj->getPosition();
			glm::quat qu = obj->getQuaternion();
			udpClient.sendPacket(to_string(clientID) + "-" + "pos:" + to_string(pos.x) + ", "
				+ to_string(pos.y) + ","
				+ to_string(pos.z) + ","
				+ to_string(qu.x) + ","
				+ to_string(qu.y) + ","
				+ to_string(qu.z) + ","
				+ to_string(qu.w));
			this_thread::sleep_for(2ms);
		}
	}


}