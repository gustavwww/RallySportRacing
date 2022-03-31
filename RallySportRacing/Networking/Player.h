#ifndef PLAYER
#define PLAYER

#include <string>
#include <glm/glm.hpp>

#include "Game/GameObject.h"

using namespace std;

namespace Networking {

	class Player {
	public:
		Player(string name, Game::GameObject* obj);
		void setPosition(glm::vec3 pos);
		void setOrientation(glm::vec3 orientation);

	private:
		string name;
		Game::GameObject* obj;
	};

}



#endif