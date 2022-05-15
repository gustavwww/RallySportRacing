#include "PlayerData.h"
#include <BulletSoftBody/btSoftBody.h>

using namespace std;

namespace Networking {

	PlayerData::PlayerData(Protocol::Command cmd, int index) {

		float posX = stof(cmd.getArgs()[index + 1]);
		float posY = stof(cmd.getArgs()[index + 2]);
		float posZ = stof(cmd.getArgs()[index + 3]);

		pos = glm::vec3(posX, posY, posZ);

		float quX = stof(cmd.getArgs()[index + 4]);
		float quY = stof(cmd.getArgs()[index + 5]);
		float quZ = stof(cmd.getArgs()[index + 6]);
		float quW = stof(cmd.getArgs()[index + 7]);

		orientation = glm::quat(quW, quX, quY, quZ);

		speed = stof(cmd.getArgs()[index + 8]);

		float velX = stof(cmd.getArgs()[index + 9]);
		float velY = stof(cmd.getArgs()[index + 10]);
		float velZ = stof(cmd.getArgs()[index + 11]);

		velocity = glm::vec3(velX, velY, velZ);

		soundString = cmd.getArgs()[index + 12];

		posX = stof(cmd.getArgs()[index + 13]);
		posY = stof(cmd.getArgs()[index + 14]);
		posZ = stof(cmd.getArgs()[index + 15]);
		
		quX = stof(cmd.getArgs()[index + 16]);
		quY = stof(cmd.getArgs()[index + 17]);
		quZ = stof(cmd.getArgs()[index + 18]);
		quW = stof(cmd.getArgs()[index + 19]);

		frontLeftPos = glm::vec3(posX, posY, posZ);
		frontLeftOr = glm::quat(quW, quX, quY, quZ);

		posX = stof(cmd.getArgs()[index + 20]);
		posY = stof(cmd.getArgs()[index + 21]);
		posZ = stof(cmd.getArgs()[index + 22]);

		quX = stof(cmd.getArgs()[index + 23]);
		quY = stof(cmd.getArgs()[index + 24]);
		quZ = stof(cmd.getArgs()[index + 25]);
		quW = stof(cmd.getArgs()[index + 26]);

		frontRightPos = glm::vec3(posX, posY, posZ);
		frontRightOr = glm::quat(quW, quX, quY, quZ);

		posX = stof(cmd.getArgs()[index + 27]);
		posY = stof(cmd.getArgs()[index + 28]);
		posZ = stof(cmd.getArgs()[index + 29]);

		quX = stof(cmd.getArgs()[index + 30]);
		quY = stof(cmd.getArgs()[index + 31]);
		quZ = stof(cmd.getArgs()[index + 32]);
		quW = stof(cmd.getArgs()[index + 33]);

		backLeftPos = glm::vec3(posX, posY, posZ);
		backLeftOr = glm::quat(quW, quX, quY, quZ);

		posX = stof(cmd.getArgs()[index + 34]);
		posY = stof(cmd.getArgs()[index + 35]);
		posZ = stof(cmd.getArgs()[index + 36]);

		quX = stof(cmd.getArgs()[index + 37]);
		quY = stof(cmd.getArgs()[index + 38]);
		quZ = stof(cmd.getArgs()[index + 39]);
		quW = stof(cmd.getArgs()[index + 40]);

		backRightPos = glm::vec3(posX, posY, posZ);
		backRightOr = glm::quat(quW, quX, quY, quZ);

	}

}