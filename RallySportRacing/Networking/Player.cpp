#include "Player.h"
#include <glm/glm.hpp>

using namespace std;

namespace Networking {

	Player::Player(Rendering::SDLWindowHandler* windowHandler, Networking::PlayerData playerData) {
		
		name = playerData.name;

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
		delete obj;
	}

	string Player::getName() {
		return name;
	}

	Rendering::Model* Player::getModel() {
		return obj->getModel();
	}

}