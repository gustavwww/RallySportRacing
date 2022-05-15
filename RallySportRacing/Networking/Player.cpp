#include "Player.h"
#include "Game/GameManager.h"
#include <glm/glm.hpp>

using namespace std;

namespace Networking {

	Player::Player(Rendering::SDLWindowHandler* windowHandler, string name, int color) {
		this->windowHandler = windowHandler;
		this->name = name;
		this->color = color;

		velocity = glm::vec3(0.0f);
	}

	void Player::initPlayer(PlayerData data) {

		label = new Rendering::Text2D(name, glm::vec3(0.5, 0.8f, 0.2f), data.pos);
		windowHandler->addText(label);

		Rendering::Model* model = Rendering::Model::loadModel("../Models/PorscheGT3.gltf", false, true);
		windowHandler->addModel(model);

		obj = new Game::GameObject(model);
		obj->setPosition(data.pos);
		obj->setQuaternion(data.orientation);

		Rendering::Model* frontLeftModel = Rendering::Model::loadModel("../Models/TwoSidedWheel.gltf", false, true);
		windowHandler->addModel(frontLeftModel);
		frontLeft = new Game::GameObject(frontLeftModel);
		frontLeft->setPosition(data.frontLeftPos);
		frontLeft->setQuaternion(data.frontLeftOr);

		Rendering::Model* frontRightModel = Rendering::Model::loadModel("../Models/TwoSidedWheel.gltf", false, true);
		windowHandler->addModel(frontRightModel);
		frontRight = new Game::GameObject(frontRightModel);
		frontRight->setPosition(data.frontRightPos);
		frontRight->setQuaternion(data.frontRightOr);

		Rendering::Model* backLeftModel = Rendering::Model::loadModel("../Models/TwoSidedWheel.gltf", false, true);
		windowHandler->addModel(backLeftModel);
		backLeft = new Game::GameObject(backLeftModel);
		backLeft->setPosition(data.backLeftPos);
		backLeft->setQuaternion(data.backLeftOr);

		Rendering::Model* backRightModel = Rendering::Model::loadModel("../Models/TwoSidedWheel.gltf", false, true);
		windowHandler->addModel(backRightModel);
		backRight = new Game::GameObject(backRightModel);
		backRight->setPosition(data.backRightPos);
		backRight->setQuaternion(data.backRightOr);

		Game::addPlayerColorUpdate(this);

		isInitialized = true;
	}

	void Player::updateState(PlayerData data) {
		if (!isInitialized) {
			initPlayer(data);
			return;
		}

		//label->setText(name);

		velocity = data.velocity;
		label->updatePos(data.pos);
		obj->setPosition(data.pos);
		obj->setQuaternion(data.orientation);
		frontLeft->setPosition(data.frontLeftPos);
		frontLeft->setQuaternion(data.frontLeftOr);
		frontRight->setPosition(data.frontRightPos);
		frontRight->setQuaternion(data.frontRightOr);
		backLeft->setPosition(data.backLeftPos);
		backLeft->setQuaternion(data.backLeftOr);
		backRight->setPosition(data.backRightPos);
		backRight->setQuaternion(data.backRightOr);
	}

	void Player::subUpdate(float deltaTime) {
		if (!isInitialized) {
			return;
		}

		glm::vec3 oldPos = obj->getPosition();
		glm::vec3 newPos = oldPos + velocity * deltaTime;
		obj->setPosition(newPos);

		frontLeft->setPosition(frontLeft->getPosition() + velocity * deltaTime);
		frontRight->setPosition(frontRight->getPosition() + velocity * deltaTime);
		backLeft->setPosition(backLeft->getPosition() + velocity * deltaTime);
		backRight->setPosition(backRight->getPosition() + velocity * deltaTime);
	}

	Player::~Player() {
		windowHandler->removeText(label);
		windowHandler->removeModel(obj->getModel());
		windowHandler->removeModel(frontLeft->getModel());
		windowHandler->removeModel(frontRight->getModel());
		windowHandler->removeModel(backLeft->getModel());
		windowHandler->removeModel(backRight->getModel());
		/*
		delete obj;
		delete frontLeft;
		delete frontRight;
		delete backLeft;
		delete backRight;
		*/
	}

	void Player::setName(string name) {
		this->name = name;
	}

	void Player::updateColor() {
		if (!isInitialized) {
			return;
		}

		obj->getModel()->updateMaterial(color, "chassiColor");
	}

	void Player::setColor(int color) {
		this->color = color;
	}

	string Player::getName() {
		return name;
	}

	int Player::getColor() {
		return color;
	}

}