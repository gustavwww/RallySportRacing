#ifndef PLAYER
#define PLAYER

#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Game/GameObject.h"

using namespace std;

namespace Networking {

	class Player {
	public:
		Player(string name, Game::GameObject* obj);
		void setPosition(glm::vec3 pos);
		void setQuaternion(glm::quat quaternion);

	private:
		string name;
		Game::GameObject* obj;
	};

}



#endif