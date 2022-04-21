#ifndef PLAYERDATA
#define PLAYERDATA

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Services/Protocol/Command.h"

using namespace std;

namespace Networking {

	class PlayerData {
	public:
		PlayerData(Protocol::Command cmd, int index);
		string name;
		glm::vec3 pos;
		glm::quat orientation;

		glm::vec3 frontLeftPos;
		glm::quat frontLeftOr;
		glm::vec3 frontRightPos;
		glm::quat frontRightOr;

		glm::vec3 backLeftPos;
		glm::quat backLeftOr;
		glm::vec3 backRightPos;
		glm::quat backRightOr;

	};

}


#endif