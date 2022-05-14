#include "PlayerData.h"
#include <BulletSoftBody/btSoftBody.h>

using namespace std;

namespace Networking {

	PlayerData::PlayerData(Protocol::Command cmd, int index) {

		name = cmd.getArgs()[index + 1];
		color = cmd.getArgs()[index + 2];
		float posX = stof(cmd.getArgs()[index + 3]);
		float posY = stof(cmd.getArgs()[index + 4]);
		float posZ = stof(cmd.getArgs()[index + 5]);

		pos = glm::vec3(posX, posY, posZ);

		float quX = stof(cmd.getArgs()[index + 6]);
		float quY = stof(cmd.getArgs()[index + 7]);
		float quZ = stof(cmd.getArgs()[index + 8]);
		float quW = stof(cmd.getArgs()[index + 9]);

		orientation = glm::quat(quW, quX, quY, quZ);

		speed = stof(cmd.getArgs()[index + 10]);

		float velX = stof(cmd.getArgs()[index + 11]);
		float velY = stof(cmd.getArgs()[index + 12]);
		float velZ = stof(cmd.getArgs()[index + 13]);

		velocity = glm::vec3(velX, velY, velZ);

		soundString = cmd.getArgs()[index + 14];

		posX = stof(cmd.getArgs()[index + 15]);
		posY = stof(cmd.getArgs()[index + 16]);
		posZ = stof(cmd.getArgs()[index + 17]);
		
		quX = stof(cmd.getArgs()[index + 18]);
		quY = stof(cmd.getArgs()[index + 19]);
		quZ = stof(cmd.getArgs()[index + 20]);
		quW = stof(cmd.getArgs()[index + 21]);

		frontLeftPos = glm::vec3(posX, posY, posZ);
		frontLeftOr = glm::quat(quW, quX, quY, quZ);

		posX = stof(cmd.getArgs()[index + 22]);
		posY = stof(cmd.getArgs()[index + 23]);
		posZ = stof(cmd.getArgs()[index + 24]);

		quX = stof(cmd.getArgs()[index + 25]);
		quY = stof(cmd.getArgs()[index + 26]);
		quZ = stof(cmd.getArgs()[index + 27]);
		quW = stof(cmd.getArgs()[index + 28]);

		frontRightPos = glm::vec3(posX, posY, posZ);
		frontRightOr = glm::quat(quW, quX, quY, quZ);

		posX = stof(cmd.getArgs()[index + 29]);
		posY = stof(cmd.getArgs()[index + 30]);
		posZ = stof(cmd.getArgs()[index + 31]);

		quX = stof(cmd.getArgs()[index + 32]);
		quY = stof(cmd.getArgs()[index + 33]);
		quZ = stof(cmd.getArgs()[index + 34]);
		quW = stof(cmd.getArgs()[index + 35]);

		backLeftPos = glm::vec3(posX, posY, posZ);
		backLeftOr = glm::quat(quW, quX, quY, quZ);

		posX = stof(cmd.getArgs()[index + 36]);
		posY = stof(cmd.getArgs()[index + 37]);
		posZ = stof(cmd.getArgs()[index + 38]);

		quX = stof(cmd.getArgs()[index + 39]);
		quY = stof(cmd.getArgs()[index + 40]);
		quZ = stof(cmd.getArgs()[index + 41]);
		quW = stof(cmd.getArgs()[index + 42]);

		backRightPos = glm::vec3(posX, posY, posZ);
		backRightOr = glm::quat(quW, quX, quY, quZ);

	}

}