#ifndef NETWORKING
#define NETWORKING

#include <glm/glm.hpp>
#include <string>

namespace Networking {

	void setupNetwork();
	void joinGame(string id, string name, Game::GameObject gameObj);
	void tcpPacketReceived();
	void sendPosition(glm::vec3 position);

}

#endif