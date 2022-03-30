#include "Player.h"
#include <glm/glm.hpp>

using namespace std;

namespace Networking {

	Player::Player(string name, Game::GameObject* obj) {
		this->name = name;
		this->obj = obj;
	}

	void Player::setPosition(glm::vec3 pos) {
		obj->setPosition(pos);
	}

}