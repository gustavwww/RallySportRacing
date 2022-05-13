#ifndef GAME
#define GAME

#include <string>
#include "GameObject.h"
#include "Rendering/SDLWindowHandler.h"

namespace Game {
	void activateSnow();
	void activateRain();
	void initCheckPoints();
	void setupGame(Rendering::SDLWindowHandler* windowHandler);
	void update();
	void adjustCamPosition();
	btVector3 glmToBullet(const glm::vec3& vec);
	btQuaternion glmToBullet(const glm::quat& q);
	glm::vec3 bulletToGlm(const btVector3& v);
	glm::quat bulletToGlm(const btQuaternion& q);
	tuple<float, float, float> glmToTuple(const glm::vec3& vec);
	void drawDebug();
	void applicationExit();
	Rendering::SDLWindowHandler* getHandler();
	float getRaceTime();
	float getCountDownTime();
	float getVehicleSpeed();
	bool getCountDown();
	int getCheckpointsReached();
	void setTextureIndex(int a);
}

#endif