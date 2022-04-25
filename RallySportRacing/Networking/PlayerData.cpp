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

		speed = stof(cmd.getArgs()[index + 9]);

		soundString = cmd.getArgs()[index + 9];

		posX = stof(cmd.getArgs()[index + 10]);
		posY = stof(cmd.getArgs()[index + 11]);
		posZ = stof(cmd.getArgs()[index + 12]);
		
		quX = stof(cmd.getArgs()[index + 13]);
		quY = stof(cmd.getArgs()[index + 14]);
		quZ = stof(cmd.getArgs()[index + 15]);
		quW = stof(cmd.getArgs()[index + 16]);

		frontLeftPos = glm::vec3(posX, posY, posZ);
		frontLeftOr = glm::quat(quW, quX, quY, quZ);

		posX = stof(cmd.getArgs()[index + 17]);
		posY = stof(cmd.getArgs()[index + 18]);
		posZ = stof(cmd.getArgs()[index + 19]);

		quX = stof(cmd.getArgs()[index + 20]);
		quY = stof(cmd.getArgs()[index + 21]);
		quZ = stof(cmd.getArgs()[index + 22]);
		quW = stof(cmd.getArgs()[index + 23]);

		frontRightPos = glm::vec3(posX, posY, posZ);
		frontRightOr = glm::quat(quW, quX, quY, quZ);

		posX = stof(cmd.getArgs()[index + 24]);
		posY = stof(cmd.getArgs()[index + 25]);
		posZ = stof(cmd.getArgs()[index + 26]);

		quX = stof(cmd.getArgs()[index + 27]);
		quY = stof(cmd.getArgs()[index + 28]);
		quZ = stof(cmd.getArgs()[index + 29]);
		quW = stof(cmd.getArgs()[index + 30]);

		backLeftPos = glm::vec3(posX, posY, posZ);
		backLeftOr = glm::quat(quW, quX, quY, quZ);

		posX = stof(cmd.getArgs()[index + 31]);
		posY = stof(cmd.getArgs()[index + 32]);
		posZ = stof(cmd.getArgs()[index + 33]);

		quX = stof(cmd.getArgs()[index + 34]);
		quY = stof(cmd.getArgs()[index + 35]);
		quZ = stof(cmd.getArgs()[index + 36]);
		quW = stof(cmd.getArgs()[index + 37]);

		backRightPos = glm::vec3(posX, posY, posZ);
		backRightOr = glm::quat(quW, quX, quY, quZ);

	}

}