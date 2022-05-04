#include "PlayerData.h"
#include <BulletSoftBody/btSoftBody.h>

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

		float velocityX = stof(cmd.getArgs()[index + 10]);
		float velocityY = stof(cmd.getArgs()[index + 11]);
		float velocityZ = stof(cmd.getArgs()[index + 12]);

		velocity = make_tuple(velocityX, velocityY, velocityZ);

		soundString = cmd.getArgs()[index + 13];

		posX = stof(cmd.getArgs()[index + 14]);
		posY = stof(cmd.getArgs()[index + 15]);
		posZ = stof(cmd.getArgs()[index + 16]);
		
		quX = stof(cmd.getArgs()[index + 17]);
		quY = stof(cmd.getArgs()[index + 18]);
		quZ = stof(cmd.getArgs()[index + 19]);
		quW = stof(cmd.getArgs()[index + 20]);

		frontLeftPos = glm::vec3(posX, posY, posZ);
		frontLeftOr = glm::quat(quW, quX, quY, quZ);

		posX = stof(cmd.getArgs()[index + 21]);
		posY = stof(cmd.getArgs()[index + 22]);
		posZ = stof(cmd.getArgs()[index + 23]);

		quX = stof(cmd.getArgs()[index + 24]);
		quY = stof(cmd.getArgs()[index + 25]);
		quZ = stof(cmd.getArgs()[index + 26]);
		quW = stof(cmd.getArgs()[index + 27]);

		frontRightPos = glm::vec3(posX, posY, posZ);
		frontRightOr = glm::quat(quW, quX, quY, quZ);

		posX = stof(cmd.getArgs()[index + 28]);
		posY = stof(cmd.getArgs()[index + 29]);
		posZ = stof(cmd.getArgs()[index + 30]);

		quX = stof(cmd.getArgs()[index + 31]);
		quY = stof(cmd.getArgs()[index + 32]);
		quZ = stof(cmd.getArgs()[index + 33]);
		quW = stof(cmd.getArgs()[index + 34]);

		backLeftPos = glm::vec3(posX, posY, posZ);
		backLeftOr = glm::quat(quW, quX, quY, quZ);

		posX = stof(cmd.getArgs()[index + 35]);
		posY = stof(cmd.getArgs()[index + 36]);
		posZ = stof(cmd.getArgs()[index + 37]);

		quX = stof(cmd.getArgs()[index + 38]);
		quY = stof(cmd.getArgs()[index + 39]);
		quZ = stof(cmd.getArgs()[index + 40]);
		quW = stof(cmd.getArgs()[index + 41]);

		backRightPos = glm::vec3(posX, posY, posZ);
		backRightOr = glm::quat(quW, quX, quY, quZ);

	}

}