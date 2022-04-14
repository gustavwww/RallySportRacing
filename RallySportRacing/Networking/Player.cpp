#include "Player.h"
#include <glm/glm.hpp>

using namespace std;

namespace Networking {

	Player::Player(string name, Game::GameObject* obj) {
		this->name = name;
		this->obj = obj;
	}

	Player::~Player() {
		delete obj;
	}

	string Player::getName() {
		return name;
	}

	void Player::setPosition(glm::vec3 pos) {
		obj->setPosition(pos);
	}
	void Player::setQuaternion(glm::quat quaternion) {
		obj->setQuaternion(quaternion);
	}

	Rendering::Model* Player::getModel() {
		return obj->getModel();
	}

}