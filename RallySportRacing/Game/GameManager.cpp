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


	chrono::steady_clock::time_point previousTime;
	void update() {
		// Called before every render.
		// TODO: Take inputs to perform transformations on objects.

		// Satte en tillfällig delta time eftersom jag inte fick den vanliga att fungera.
		float dt = 0.01f;
		cube->translate(glm::vec3(0, 1, 3) * dt);


		previousTime = chrono::high_resolution_clock::now();
	}

	float deltaTime() {
		return chrono::duration<float, milli>(chrono::high_resolution_clock::now() - previousTime).count() * 0.001;
	}

}