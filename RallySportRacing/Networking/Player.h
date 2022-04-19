#ifndef PLAYER
#define PLAYER

#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Game/GameObject.h"
#include "Rendering/Model.h"

using namespace std;

namespace Networking {

	class Player {
	public:
		Player(string name, Game::GameObject* obj);
		~Player();

		string getName();
		void setPosition(glm::vec3 pos);
		void setQuaternion(glm::quat quaternion);
		Rendering::Model* getModel();

	private:
		string name;
		Game::GameObject* obj;
	};

}



#endif