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
#include <glm/gtx/quaternion.hpp>
#include <Audio/audio.h>

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

	Audio* sound;

	map<int, Player*> players;

	void setupNetwork(Game::Vehicle* playerObj, Rendering::SDLWindowHandler* windowHandler) {
		vehicle = playerObj;
		handler = windowHandler;

		tcpClient.addCallback(Networking::tcpPacketReceived);
		tcpClient.connectToServer();

		udpClient.addCallback(Networking::udpPacketReceived);

		tcpThread = thread(&Server::TCPClient::listen, tcpClient);
		udpThread = thread(&Server::UDPClient::listen, udpClient);

		sound = Audio::Instance();
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

			if (cmd.getArgsSize() % 43 != 0) {
				// Skip corrupt UDP packets.
				return;
			}

			// Store players already in game to eventuelly remove them.
			vector<int> playersInGame;
			for (auto el : players)
				playersInGame.push_back(el.first);
			
			for (int i = 0; i < cmd.getArgsSize(); i++) {
				if (cmd.getArgs()[i] == "player" && (i+42) <= cmd.getArgsSize()) {
					i++;

					int id = stoi(cmd.getArgs()[i]);
					if (id == clientID) {
						continue;
					}

					PlayerData data(cmd, i);

					auto el = players.find(id);
					if (el == players.end()) {
						// Player not initialized, creating player...
					
						Player* p = new Player(handler, data);
						players.insert(pair<int, Player*>(id, p));
						cout << "A player has joined the game: " << p->getName() << endl;

						// Create sound source
						sound->createSoundSource(id, data.pos);
					}
					else {
						// Player already created, updating position...
						Player* p = el->second;
						p->updateState(data);
						playersInGame.erase(find(playersInGame.begin(), playersInGame.end(), id));

						// Update sound source
						sound->updateSoundSource(id, data.pos, data.velocity, data.speed, data.soundString);
					}

				}

			}

			// Remove players no longer in game.
			for (int id : playersInGame) {
				auto el = players.find(id);
				if (el != players.end()) {
					Player* p = el->second;
					players.erase(el);
					cout << "A player has left the game: " << p->getName() << endl;
					// TODO: Fix player deletion, currently kills the game..
					//delete p;

					// Remove sound source
					sound->removeSoundSource(id);
					delete p;
				}
			}

		}

	}

	void sendStatusPacket() {
		while (inGame) {
			glm::vec3 pos = vehicle->getPosition();
			glm::quat qu = vehicle->getQuaternion();
			glm::vec3 vel = vehicle->getVelocity();
			glm::vec3 frontLeftPos = vehicle->wheel1->getPosition();
			glm::quat frontLeftOr = vehicle->wheel1->getQuaternion();
			glm::vec3 frontRightPos = vehicle->wheel2->getPosition();
			glm::quat frontRightOr = vehicle->wheel2->getQuaternion();
			glm::vec3 backLeftPos = vehicle->wheel3->getPosition();
			glm::quat backLeftOr = vehicle->wheel3->getQuaternion();
			glm::vec3 backRightPos = vehicle->wheel4->getPosition();
			glm::quat backRightOr = vehicle->wheel4->getQuaternion();
			udpClient.sendPacket(to_string(clientID) + "-" + "pos:" + to_string(pos.x) + ", "
				+ to_string(pos.y) + ","
				+ to_string(pos.z) + ","
				+ to_string(qu.x) + ","
				+ to_string(qu.y) + ","
				+ to_string(qu.z) + ","
				+ to_string(qu.w) + ","
				+ to_string(vehicle->getSpeed()) + ","
				+ to_string(vel.x) + ","
				+ to_string(vel.y) + ","
				+ to_string(vel.z) + ","
				+ sound->getSoundString(0) + ","
				+ to_string(frontLeftPos.x) + ","
				+ to_string(frontLeftPos.y) + ","
				+ to_string(frontLeftPos.z) + ","
				+ to_string(frontLeftOr.x) + ","
				+ to_string(frontLeftOr.y) + ","
				+ to_string(frontLeftOr.z) + ","
				+ to_string(frontLeftOr.w) + ","
				+ to_string(frontRightPos.x) + ","
				+ to_string(frontRightPos.y) + ","
				+ to_string(frontRightPos.z) + ","
				+ to_string(frontRightOr.x) + ","
				+ to_string(frontRightOr.y) + ","
				+ to_string(frontRightOr.z) + ","
				+ to_string(frontRightOr.w) + ","
				+ to_string(backLeftPos.x) + ","
				+ to_string(backLeftPos.y) + ","
				+ to_string(backLeftPos.z) + ","
				+ to_string(backLeftOr.x) + ","
				+ to_string(backLeftOr.y) + ","
				+ to_string(backLeftOr.z) + ","
				+ to_string(backLeftOr.w) + ","
				+ to_string(backRightPos.x) + ","
				+ to_string(backRightPos.y) + ","
				+ to_string(backRightPos.z) + ","
				+ to_string(backRightOr.x) + ","
				+ to_string(backRightOr.y) + ","
				+ to_string(backRightOr.z) + ","
				+ to_string(backRightOr.w)
			);

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