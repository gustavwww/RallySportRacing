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

		posX = stof(cmd.getArgs()[index + 9]);
		posY = stof(cmd.getArgs()[index + 10]);
		posZ = stof(cmd.getArgs()[index + 11]);

		quX = stof(cmd.getArgs()[index + 12]);
		quY = stof(cmd.getArgs()[index + 13]);
		quZ = stof(cmd.getArgs()[index + 14]);
		quW = stof(cmd.getArgs()[index + 15]);

		frontLeftPos = glm::vec3(posX, posY, posZ);
		frontLeftOr = glm::quat(quW, quX, quY, quZ);

		posX = stof(cmd.getArgs()[index + 16]);
		posY = stof(cmd.getArgs()[index + 17]);
		posZ = stof(cmd.getArgs()[index + 18]);

		quX = stof(cmd.getArgs()[index + 19]);
		quY = stof(cmd.getArgs()[index + 20]);
		quZ = stof(cmd.getArgs()[index + 21]);
		quW = stof(cmd.getArgs()[index + 22]);

		frontRightPos = glm::vec3(posX, posY, posZ);
		frontRightOr = glm::quat(quW, quX, quY, quZ);

		posX = stof(cmd.getArgs()[index + 23]);
		posY = stof(cmd.getArgs()[index + 24]);
		posZ = stof(cmd.getArgs()[index + 25]);

		quX = stof(cmd.getArgs()[index + 26]);
		quY = stof(cmd.getArgs()[index + 27]);
		quZ = stof(cmd.getArgs()[index + 28]);
		quW = stof(cmd.getArgs()[index + 29]);

		backLeftPos = glm::vec3(posX, posY, posZ);
		backLeftOr = glm::quat(quW, quX, quY, quZ);

		posX = stof(cmd.getArgs()[index + 30]);
		posY = stof(cmd.getArgs()[index + 31]);
		posZ = stof(cmd.getArgs()[index + 32]);

		quX = stof(cmd.getArgs()[index + 33]);
		quY = stof(cmd.getArgs()[index + 34]);
		quZ = stof(cmd.getArgs()[index + 35]);
		quW = stof(cmd.getArgs()[index + 36]);

		backRightPos = glm::vec3(posX, posY, posZ);
		backRightOr = glm::quat(quW, quX, quY, quZ);

	}

}