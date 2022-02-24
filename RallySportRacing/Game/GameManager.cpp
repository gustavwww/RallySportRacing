#include "GameManager.h"
#include <chrono>
#include <iostream>
#include "Rendering/SDLWindowHandler.h"

using namespace std;

namespace Game {

	GameObject* car1;
	GameObject* car2;
	Rendering::SDLWindowHandler* handler;


	void setupGame(Rendering::SDLWindowHandler* windowHandler) {

		handler = windowHandler;

		Rendering::Model* carModel1 = Rendering::Model::createModel("../Models/SimpleCarAppliedTransforms.gltf");
		windowHandler->addModel(carModel1);
		car1 = new GameObject(carModel1);

		Rendering::Model* carModel2 = Rendering::Model::createModel("../Models/SimpleCarAppliedTransforms.gltf");
		windowHandler->addModel(carModel2);
		car2 = new GameObject(carModel2);
	}

	bool toScreen = true;
	bool firstTime = true;
	chrono::steady_clock::time_point previousTime;
	chrono::steady_clock::time_point currentTime;


	const Uint8* keyboard_state_array = SDL_GetKeyboardState(NULL);

	glm::vec3 angleVector;
	glm::vec3 directionVector;
	float speed = 10;

	glm::vec3 camPosition;
	glm::vec3 camDirection;
	glm::vec3 camOffsetVector;
	glm::vec3 camOffset;

	void update() {

		// Calculate deltaTime
		if (firstTime) {
			firstTime = false;
			currentTime = chrono::high_resolution_clock::now();
		}
		previousTime = currentTime;
		currentTime = chrono::high_resolution_clock::now();
		float deltaTime = chrono::duration<float, milli>(currentTime - previousTime).count() * 0.001;

		// Called before every render.
		// TODO:
		// gonna fix the code so it is more simple and clear
		// fix reversing, wrong inputs. It works like it should but it does not act like a car with wheels. This is why the reversing is inversed.
		SDL_PumpEvents();
		if (keyboard_state_array[SDL_SCANCODE_W])
		{
			car1->translate(directionVector * deltaTime * speed);
		}
		if (keyboard_state_array[SDL_SCANCODE_S])
		{
			car1->translate(directionVector * glm::vec3(-1, 1, -1) * deltaTime * speed);
		}
		if (keyboard_state_array[SDL_SCANCODE_D])
		{
			angleVector.x -= 3.14159f / 2.0f * deltaTime;
			car1->rotate(angleVector);
		}
		if (keyboard_state_array[SDL_SCANCODE_A])
		{
			angleVector.x += 3.14159f / 2.0f * deltaTime;
			car1->rotate(angleVector);
		}
		directionVector.x = sin(angleVector.x);
		directionVector.z = cos(angleVector.x);
		adjustCamPosition();
	}

	void adjustCamPosition() {
		camOffsetVector = directionVector * glm::vec3(-1, 1, -1);
		camOffset = glm::vec3(20 * camOffsetVector.x, 5, 20 * camOffsetVector.z); //offset 20. Height 5
		camPosition = camOffset + car1->getPosition();
		camDirection = car1->getPosition();
		handler->setCamPosition(camPosition);
		handler->setCamDirection(camDirection);
	}

}