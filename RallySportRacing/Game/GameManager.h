#ifndef GAME
#define GAME

#include "GameObject.h"
#include "Rendering/SDLWindowHandler.h"

namespace Game {

	void setupGame(Rendering::SDLWindowHandler* windowHandler);
	void update();
	void adjustCamPosition();
	btVector3 glmToBullet(const glm::vec3& vec);
	btQuaternion glmToBullet(const glm::quat& q);
	glm::vec3 bulletToGlm(const btVector3& v);
	glm::quat bulletToGlm(const btQuaternion& q);
	void drawDebug();
}

#endif