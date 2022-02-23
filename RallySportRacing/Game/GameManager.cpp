#include "GameManager.h"
#include <chrono>
#include <iostream>
#include "Rendering/SDLWindowHandler.h"

using namespace std;

namespace Game {

	GameObject* cube;
	GameObject* cube2;
	Rendering::SDLWindowHandler* handler;


	void setupGame(Rendering::SDLWindowHandler* windowHandler) {

		handler = windowHandler;

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
		// TODO: Take inputs to perform transformations on objects.

		SDL_PumpEvents();
		if (keyboard_state_array[SDL_SCANCODE_W])
		{
			directionVector.x = sin(angleVector.x);
			directionVector.z = cos(angleVector.x);
			cube->translate(directionVector * deltaTime * speed);
			//cube->changeView(directionVector * glm::vec3(-1, 1, -1));
			//cube2->changeView(directionVector);
			
			//set view som ändrar viewen i sdlwindowhandlern
			// Params: Cam pos in World Space, where to look at, head up (0,-1,0) = upside down.


		}
		if (keyboard_state_array[SDL_SCANCODE_S])
		{
			directionVector.x = sin(angleVector.x);
			directionVector.z = cos(angleVector.x);
			cube->translate(directionVector * glm::vec3(-1, 1, -1) * deltaTime * speed);
			//cube->changeView(directionVector * glm::vec3(-1, 1, -1));
			//cube2->changeView(directionVector);
		}
		if (keyboard_state_array[SDL_SCANCODE_D])
		{
			//cube->translate(glm::vec3(-20.0f, 0.0f, 0.0f) * deltaTime());
			angleVector.x -= 3.14159f / 2.0f * deltaTime;
			cube->rotate(angleVector);
			directionVector.x = sin(angleVector.x);
			directionVector.z = cos(angleVector.x);
			//cube->changeView(directionVector * glm::vec3(-1, 1, -1));
			//cube2->changeView(directionVector);
		}
		if (keyboard_state_array[SDL_SCANCODE_A])
		{
			//cube->translate(glm::vec3(20.0f, 0.0f, 0.0f) * deltaTime());
			angleVector.x += 3.14159f / 2.0f * deltaTime;
			cube->rotate(angleVector);
			directionVector.x = sin(angleVector.x);
			directionVector.z = cos(angleVector.x);
			//cube->changeView(directionVector * glm::vec3(-1, 1, -1));
			//cube2->changeView(directionVector);
		}
		camOffsetVector = directionVector * glm::vec3(-1, 1, -1);
		camOffset = glm::vec3(20 * camOffsetVector.x, 5, 20 * camOffsetVector.z); //offset 20
		camPosition = camOffset + cube->getPosition();
		camDirection = cube->getPosition();
		handler->setCamPosition(camPosition);
		handler->setCamDirection(camDirection);

		/*SDL_Event event;
		while (SDL_PollEvent(&event)){
			if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
				if (keyboard_state_array[SDL_SCANCODE_W] && !(keyboard_state_array[SDL_SCANCODE_S]))
				{
					cube->translate(glm::vec3(0.0f, 20.0f, 0.0f) * deltaTime()); 
				}
				else if (!keyboard_state_array[SDL_SCANCODE_W] && keyboard_state_array[SDL_SCANCODE_S])
				{
					cube->translate(glm::vec3(0.0f, -20.0f, 0.0f) * deltaTime());
				}

				if (keyboard_state_array[SDL_SCANCODE_D] && !keyboard_state_array[SDL_SCANCODE_A])
				{
					cube->translate(glm::vec3(20.0f, 0.0f, 0.0f) * deltaTime());
				}
				else if (!keyboard_state_array[SDL_SCANCODE_D] && keyboard_state_array[SDL_SCANCODE_A])
				{
					cube->translate(glm::vec3(-20.0f, 0.0f, 0.0f) * deltaTime());
				}
			}
		}
		*/

		// TODO: Better fix for first-time delta time :(

		if (cube2->getPosition().x >= 5 || cube2->getPosition().x <= -5) {
			toScreen = !toScreen;
		}

		if (toScreen) {
			//cube->translate(glm::vec3(7.0f, -7.0f, 5.0f) * deltaTime());
			cube2->translate(glm::vec3(-7.0f, 7.0f, 5.0f) * deltaTime);
		} else {
			//cube->translate(glm::vec3(-7.0f, 7.0f, -5.0f) * deltaTime());
			cube2->translate(glm::vec3(7.0f, -7.0f, -5.0f) * deltaTime);
		}
		

		//previousTime = chrono::high_resolution_clock::now();
	}

}