#include "GameManager.h"
#include <chrono>
#include <iostream>

using namespace std;

namespace Game {

	GameObject* cube;
	GameObject* cube2;

	void setupGame(Rendering::SDLWindowHandler* windowHandler) {

		// Cube 1
		Rendering::Model* cubeModel = Rendering::Model::createCube();
		windowHandler->addModel(cubeModel);
		cube = new GameObject(cubeModel);

		// Cube 2
		Rendering::Model* cubeModel2 = Rendering::Model::createCube();
		windowHandler->addModel(cubeModel2);
		cube2 = new GameObject(cubeModel2);
	}

	bool toScreen = true;
	bool firstTime = true;
	chrono::steady_clock::time_point previousTime;
	void update() {
		// Called before every render.
		// TODO: Take inputs to perform transformations on objects.

		// TODO: Better fix for first-time delta time :(
		if (firstTime) {
			firstTime = false;
			previousTime = chrono::high_resolution_clock::now();
		}

		if (cube->getPosition().x >= 5 || cube->getPosition().x <= -5) {
			toScreen = !toScreen;
		}

		if (toScreen) {
			cube->translate(glm::vec3(7.0f, -7.0f, 5.0f) * deltaTime());
			cube2->translate(glm::vec3(-7.0f, 7.0f, 5.0f) * deltaTime());
		} else {
			cube->translate(glm::vec3(-7.0f, 7.0f, -5.0f) * deltaTime());
			cube2->translate(glm::vec3(7.0f, -7.0f, -5.0f) * deltaTime());
		}
		

		previousTime = chrono::high_resolution_clock::now();
	}

	
	float deltaTime() {
		return chrono::duration<float, milli>(chrono::high_resolution_clock::now() - previousTime).count() * 0.001;
	}

}