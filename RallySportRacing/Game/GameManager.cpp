#include "GameManager.h"
#include <chrono>
#include <iostream>
#include <cmath>
#include "Rendering/SDLWindowHandler.h"
#include "../../External/bullet/headers/btBulletCollisionCommon.h"
#include "../../External/bullet/headers/btBulletDynamicsCommon.h"
#include <Physics/Physics.hpp>

using namespace std;

namespace Game {

	GameObject* car1;
	GameObject* environment;
	GameObject* wall;

	//Debug GameObject
	GameObject* debugEnvironment;

	Rendering::SDLWindowHandler* handler;

	//Colors to select from when creating a model
	glm::vec3 red = glm::vec3(1.0f, 0.f, 0.f);
	glm::vec3 blue = glm::vec3(0.f, 0.f, 1.f);
	glm::vec3 green = glm::vec3(0.f, 1.f, 0.f);


	void setupGame(Rendering::SDLWindowHandler* windowHandler) {

		Physics* physics = new Physics();
		handler = windowHandler;

		Rendering::Model* carModel1 = Rendering::Model::loadModel("../Models/SimpleCarAppliedTransforms.gltf");
		windowHandler->addModel(carModel1);
		car1 = new GameObject(carModel1, physics->dynamicsWorld);

		Rendering::Model* environmentModel = Rendering::Model::loadModel("../Models/SimpleEnvironment.gltf");
		windowHandler->addModel(environmentModel);
		environment = new GameObject(environmentModel, physics->dynamicsWorld);
		
		Rendering::Model* wallModel = Rendering::Model::loadModel("../Models/Wall.gltf");
		windowHandler->addModel(wallModel);
		wall = new GameObject(wallModel, physics->dynamicsWorld);
		
		//Light Debugging Environment
		Rendering::Model* debugEnvironmentModel = Rendering::Model::loadModel("../Models/LightTestEnvironment.gltf");
		windowHandler->addModel(debugEnvironmentModel);
		debugEnvironment = new GameObject(debugEnvironmentModel, physics->dynamicsWorld);
		debugEnvironment->translate(glm::vec3(-10.0f, 15.0f, 20.0f));
		debugEnvironment->rotate(glm::vec3(M_PI, M_PI/2, 0.0f));

		car1->translate(glm::vec3(-15.f, 0.5f, 0.f));
		wall->translate(glm::vec3(-25.f, 1.f, 0.f));
		wall->rotate(glm::vec3(0.0f, 0.0f, -M_PI / 2.0f));
	}

	bool toScreen = true;
	bool firstTime = true;
	chrono::steady_clock::time_point previousTime;
	chrono::steady_clock::time_point currentTime;


	const Uint8* keyboard_state_array = SDL_GetKeyboardState(NULL);

	glm::vec3 directionVector;
	float speed = 10;

	glm::vec3 camPosition;
	glm::vec3 camDirection;
	glm::vec3 camOrientation;
	glm::vec3 camOffsetVector;
	glm::vec3 camOffset;

	int perspective = 1; // 1 = normal thirdperson, 2 = from side, 3 = from a static camera above in a corner
	float radius = 10;
	float maxDistance = 30;
	float minDistance = 4;
	float perspectiveAngle = 0;

	const int WIDTH = 1920, HEIGHT = 1080;

	// variables for handling mouse events and movement
	int x, y;
	Uint32 buttons;
	float cameraSpeed = 10.0f; // 3 units / second
	float mouseSpeed = 0.25f;

	// Below are used to calculate direction of camera when using free look mode (perspective 3)
	// horizontal angle : toward -Z 
	float horizontalAngle = 0.f; //start values tested for the corner
	// vertical angle : 0, look at the horizon
	float verticalAngle = 0.f;

	void update() {
		// Called before every render.

		// Calculate deltaTime
		if (firstTime) {
			camOrientation = glm::vec3(0, 1, 0);
			firstTime = false;
			currentTime = chrono::high_resolution_clock::now();
		}
		previousTime = currentTime;
		currentTime = chrono::high_resolution_clock::now();
		float deltaTime = chrono::duration<float, milli>(currentTime - previousTime).count() * 0.001;

		// TODO:
		// gonna fix the code so it is more simple and clear?
		SDL_PumpEvents();
		buttons = SDL_GetMouseState(&x, &y);

		// Car movement
		if (((buttons & SDL_BUTTON_RMASK) != SDL_BUTTON_RMASK) || perspective != 3) {
			if (keyboard_state_array[SDL_SCANCODE_W]) {
				car1->translate(directionVector * deltaTime * speed);
			}

			if (keyboard_state_array[SDL_SCANCODE_S]) {
				car1->translate(directionVector * glm::vec3(-1, 1, -1) * deltaTime * speed);
			}

			if (keyboard_state_array[SDL_SCANCODE_D]) {
				car1->rotate(glm::vec3(-M_PI / 2.0f * deltaTime, 0.0f, 0.0f));
			}

			if (keyboard_state_array[SDL_SCANCODE_A]) {
				car1->rotate(glm::vec3(M_PI / 2.0f * deltaTime, 0.0f, 0.0f));
			}
			directionVector.x = sin(car1->getOrientation().x);
			directionVector.z = cos(car1->getOrientation().x);
		}

		// Different perspectives
		if (keyboard_state_array[SDL_SCANCODE_1]) {
			perspective = 1;
			//perspectiveAngle = tan(pow(camOffsetVector.x - camPosition.x, 2) / pow(camOffsetVector.z - camPosition.z, 2));
			//camOffset = glm::vec3(radius * cos(perspectiveAngle), 5, radius * sin(perspectiveAngle));
			camOrientation = glm::vec3(0, 1, 0);
		}
		if (keyboard_state_array[SDL_SCANCODE_2]) {
			camOffset = glm::vec3(radius * cos(perspectiveAngle), 5, radius * sin(perspectiveAngle));
			camOrientation = glm::vec3(0, 1, 0);
			perspective = 2;
		}
		if (keyboard_state_array[SDL_SCANCODE_3]) {
			perspective = 3;
			camOffset = glm::vec3(30, 15, -40); //corner 30, 15, -40
			camPosition = camOffset;
			camDirection = glm::vec3(0, 0, 0);
			camOrientation = glm::vec3(0, 1, 0);
			horizontalAngle = -0.7f; //start values tested for the corner
			verticalAngle = 6.f; //start values tested for the corner
		}

		// Camera handling for the different perspectives
		// Perspective 1 => start perspective following the car
		if (perspective == 1) {
			camOffsetVector = directionVector * glm::vec3(-1, 1, -1);
			camOffset = glm::vec3(20 * camOffsetVector.x, 5, 20 * camOffsetVector.z); //offset 20. Height 5

			// Interpolation on camdirection and position which creates a delay. More smooth camera movement. More immersive
			camDirection = camPosition + (car1->getPosition() + directionVector * glm::vec3(2, 2, 2) - camPosition) * 0.5f;
			camPosition = camPosition + (camOffset + car1->getPosition() - camPosition) * 0.025f; 
		}

		// Perspective 2 => still following the car but you can change the angle and position relative to the car
		else if (perspective == 2) {
			if (radius <= minDistance) {
				radius = minDistance;
			}
			if (radius >= maxDistance) { 
				radius = maxDistance;
			}
			if (keyboard_state_array[SDL_SCANCODE_UP]) {
				radius -= 0.1f;
				camOffset = glm::vec3(radius * cos(perspectiveAngle), 5, radius * sin(perspectiveAngle));
			}
			if (keyboard_state_array[SDL_SCANCODE_DOWN]) {
				radius += 0.1f;
				camOffset = glm::vec3(radius * cos(perspectiveAngle), 5, radius * sin(perspectiveAngle));
			}
			if (keyboard_state_array[SDL_SCANCODE_LEFT]) {
				perspectiveAngle += 0.01;
				camOffset = glm::vec3(radius * cos(perspectiveAngle), 5, radius * sin(perspectiveAngle));
			}
			if (keyboard_state_array[SDL_SCANCODE_RIGHT]) {
				perspectiveAngle -= 0.01;
				camOffset = glm::vec3(radius * cos(perspectiveAngle), 5, radius * sin(perspectiveAngle));
			}

			camPosition = camOffset + car1->getPosition();
			camDirection = car1->getPosition();
		}

		// Perspective 3 => static point for the camera, located in the corner of the map
		else if (perspective == 3) {
			if ((buttons & SDL_BUTTON_RMASK) == SDL_BUTTON_RMASK) {
				//SDL_Log("Mouse cursor is at %d, %d", x, y); //window is 800 x 600
				//SDL_Log("Mouse Button 1 (left) is pressed.");
				//SDL_Log("horizontalAngle %d, %d", cos(horizontalAngle), cos(verticalAngle));
				SDL_SetRelativeMouseMode(SDL_TRUE);
				SDL_WarpMouseInWindow(NULL, WIDTH / 2, HEIGHT / 2);
				// Compute new orientation
				horizontalAngle += mouseSpeed * deltaTime * float(WIDTH / 2 - x); // widht and height of window
				verticalAngle += mouseSpeed * deltaTime * float(HEIGHT / 2 - y);
				// Direction : Spherical coordinates to Cartesian coordinates conversion
				glm::vec3 direction(
					cos(verticalAngle) * sin(horizontalAngle),
					sin(verticalAngle),
					cos(verticalAngle) * cos(horizontalAngle)
				);
				// Right vector
				glm::vec3 right = glm::vec3(
					sin(horizontalAngle - 3.14f / 2.0f),
					0,
					cos(horizontalAngle - 3.14f / 2.0f)
				);
				// Up vector : perpendicular to both direction and right
				glm::vec3 up = glm::cross(right, direction);

				// Cam movement
				if (keyboard_state_array[SDL_SCANCODE_W]) {
					camPosition += direction * deltaTime * cameraSpeed;
				}

				if (keyboard_state_array[SDL_SCANCODE_S]) {
					camPosition -= direction * deltaTime * cameraSpeed;
				}

				if (keyboard_state_array[SDL_SCANCODE_D]) {
					camPosition += right * deltaTime * cameraSpeed;
				}

				if (keyboard_state_array[SDL_SCANCODE_A]) {
					camPosition -= right * deltaTime * cameraSpeed;
				}

				if (keyboard_state_array[SDL_SCANCODE_LSHIFT]) {
					cameraSpeed = 30.0f;
				}
				else {
					cameraSpeed = 10.0f;
				}
				camDirection = camPosition + direction;
				camOrientation = up;
			}
			if ((buttons & SDL_BUTTON_RMASK) != SDL_BUTTON_RMASK) {
				SDL_SetRelativeMouseMode(SDL_FALSE);
			}
		}

		adjustCamPosition();
	}

	void adjustCamPosition() {
		handler->setCamPosition(camPosition);
		handler->setCamDirection(camDirection);
		handler->setCamOrientation(camOrientation);
	}

}