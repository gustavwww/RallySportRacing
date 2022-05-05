#include "Player.h"
#include <glm/glm.hpp>

using namespace std;

namespace Networking {

	Player::Player(Rendering::SDLWindowHandler* windowHandler, Networking::PlayerData playerData) {
		this->windowHandler = windowHandler;

		name = playerData.name;

		label = new Rendering::Text2D(name, glm::vec3(207.0f, 63.0f, 77.0f), glm::vec2(playerData.pos.x, playerData.pos.y));
		windowHandler->addText(label);

		Rendering::Model* model = Rendering::Model::loadModel("../Models/PorscheGT3_wWheels.gltf", false);
		windowHandler->addModel(model);
		obj = new Game::GameObject(model);
		obj->setPosition(playerData.pos);
		obj->setQuaternion(playerData.orientation);
		
		Rendering::Model* frontLeftModel = Rendering::Model::loadModel("../Models/TwoSidedWheel.gltf", false);
		windowHandler->addModel(frontLeftModel);
		frontLeft = new Game::GameObject(frontLeftModel);
		frontLeft->setPosition(playerData.frontLeftPos);
		frontLeft->setQuaternion(playerData.frontLeftOr);

		Rendering::Model* frontRightModel = Rendering::Model::loadModel("../Models/TwoSidedWheel.gltf", false);
		windowHandler->addModel(frontRightModel);
		frontRight = new Game::GameObject(frontRightModel);
		frontRight->setPosition(playerData.frontRightPos);
		frontRight->setQuaternion(playerData.frontRightOr);

		Rendering::Model* backLeftModel = Rendering::Model::loadModel("../Models/TwoSidedWheel.gltf", false);
		windowHandler->addModel(backLeftModel);
		backLeft = new Game::GameObject(backLeftModel);
		backLeft->setPosition(playerData.backLeftPos);
		backLeft->setQuaternion(playerData.backLeftOr);

		Rendering::Model* backRightModel = Rendering::Model::loadModel("../Models/TwoSidedWheel.gltf", false);
		windowHandler->addModel(backRightModel);
		backRight = new Game::GameObject(backRightModel);
		backRight->setPosition(playerData.backRightPos);
		backRight->setQuaternion(playerData.backRightOr);
		
		

	}

	void Player::updateState(PlayerData data) {
		label->updatePos(glm::vec2(data.pos.x, data.pos.y));
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

	string Player::getName() {
		return name;
	}

}