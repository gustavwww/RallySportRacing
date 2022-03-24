#ifndef NETWORKING
#define NETWORKING

#include <glm/glm.hpp>
#include <string>

using namespace std;

namespace Networking {

	void setupNetwork();
	void joinGame(string id, string name);
	void tcpPacketReceived(string str);
	void sendPosition(glm::vec3 position);

}

#endif