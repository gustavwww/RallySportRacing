#include "GameManager.h"
#include <chrono>
#include <iostream>

using namespace std;

namespace Game {

	GameObject* cube;

	void setupGame(Rendering::SDLWindowHandler* windowHandler) {

		Rendering::Model* cubeModel = Rendering::Model::createCube();
		windowHandler->addModel(cubeModel);
		cube = new GameObject(cubeModel);
	}

	bool toScreen = true;
	chrono::steady_clock::time_point previousTime;
	void update() {
		// Called before every render.
		// TODO: Take inputs to perform transformations on objects.
		

		if (cube->getPosition().x >= 5 || cube->getPosition().x <= -5) {
			toScreen = !toScreen;
		}

		if (toScreen) {
			cube->translate(glm::vec3(7.0f, -7.0f, 5.0f) * deltaTime());
		} else {
			cube->translate(glm::vec3(-7.0f, 7.0f, -5.0f) * deltaTime());
		}
		

		previousTime = chrono::high_resolution_clock::now();
	}

	bool firstTime = true;
	float deltaTime() {
		if (firstTime) {
			firstTime = false;
			return 0;
		}

		return chrono::duration<float, milli>(chrono::high_resolution_clock::now() - previousTime).count() * 0.001;
	}

}