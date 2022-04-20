#include "PlayerData.h"

using namespace std;

namespace Networking {

	PlayerData::PlayerData(Protocol::Command cmd, int index) {

		name = cmd.getArgs()[index + 1];

		float posX = stof(cmd.getArgs()[index + 2]);
		float posY = stof(cmd.getArgs()[index + 3]);
		float posZ = stof(cmd.getArgs()[index + 4]);

		pos = glm::vec3(posX, posY, posZ);

		float quX = stof(cmd.getArgs()[index + 5]);
		float quY = stof(cmd.getArgs()[index + 6]);
		float quZ = stof(cmd.getArgs()[index + 7]);
		float quW = stof(cmd.getArgs()[index + 8]);

		orientation = glm::quat(quW, quX, quY, quZ);

	}

}