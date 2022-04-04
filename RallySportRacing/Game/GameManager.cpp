#include "GameManager.h"
#include <chrono>
#include <iostream>
#include <cmath>
#include <thread>
#include "Rendering/SDLWindowHandler.h"

#include <btBulletDynamicsCommon.h>
#include <Physics/Physics.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <btBulletDynamicsCommon.h>
#include "vehicle.h"

#include "Debugging/DebugDraw.h"


#include "Services/TCPClient.h";
#include "Services/Protocol/ProtocolParser.h"
#include "Services/Protocol/Command.h"
#include "Networking/Networking.h"


using namespace std;

namespace Game {

	GameObject* environment;
	GameObject* wall;

	Vehicle* vehicle;

	//Debug GameObject
	GameObject* debugEnvironment;

	Rendering::SDLWindowHandler* handler;
	Physics* physics;

	//Colors to select from when creating a model
	glm::vec3 red = glm::vec3(1.0f, 0.f, 0.f);
	glm::vec3 blue = glm::vec3(0.f, 0.f, 1.f);
	glm::vec3 green = glm::vec3(0.f, 1.f, 0.f);


	DebugDraw* debugDrawer;

	bool isWheel = 1;

	Rendering::SDLWindowHandler* Game::getHandler()
	{
		return handler;
	}

	void setupGame(Rendering::SDLWindowHandler* windowHandler) {

		physics = new Physics();
		handler = windowHandler;

		Rendering::Model* environmentModel = Rendering::Model::loadModel("../Models/SimpleEnvironment.gltf");
		windowHandler->addModel(environmentModel);
		environment = new GameObject(environmentModel,  physics->dynamicsWorld);
		Rendering::Model* wallModel = Rendering::Model::loadModel("../Models/Wall.gltf");
		windowHandler->addModel(wallModel);
		wall = new GameObject(wallModel, physics->dynamicsWorld);
		wall->setInitialPosition(btVector3(-400, -5, 0));
		wall->setInitialRotation(btQuaternion(1,0,0,1));

		//Light Debugging Environment
		Rendering::Model* debugEnvironmentModel = Rendering::Model::loadModel("../Models/LightTestEnvironment.gltf");
		windowHandler->addModel(debugEnvironmentModel);
		debugEnvironment = new GameObject(debugEnvironmentModel, physics->dynamicsWorld);
		debugEnvironment->setInitialPosition(btVector3(-200, 0, 0));

		Rendering::Model* carModel1 = Rendering::Model::loadModel("../Models/PorscheGT3_wWheels.gltf");
		windowHandler->addModel(carModel1);
		vehicle = new Vehicle(carModel1, physics->dynamicsWorld);



		debugDrawer = new DebugDraw();


		// Multiplayer setup
		Networking::setupNetwork(vehicle, windowHandler);

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
		// Should probably just be a loop that does update on all objects
		wall->updateTransform();
		debugEnvironment->updateTransform();
		environment->updateTransform();
		vehicle->updateTransform();
		//wheel1->updateTransform();
		//wheel2->updateTransform();
		//wheel3->updateTransform();
		//wheel4->updateTransform();

		// debug drawing
		physics->dynamicsWorld->setDebugDrawer(debugDrawer);
		physics->dynamicsWorld->debugDrawWorld(); 



		// Calculate deltaTime
		if (firstTime) {
			camOrientation = glm::vec3(0, 1, 0);
			firstTime = false;
			currentTime = chrono::high_resolution_clock::now();
		}
		previousTime = currentTime;
		currentTime = chrono::high_resolution_clock::now();
		float deltaTime = chrono::duration<float, milli>(currentTime - previousTime).count() * 0.001;


		SDL_PumpEvents();
		buttons = SDL_GetMouseState(&x, &y);
		if (keyboard_state_array[SDL_SCANCODE_H]) {
			//car1->updateTransform();
		}
		// Car movement
		if (((buttons & SDL_BUTTON_RMASK) != SDL_BUTTON_RMASK) || perspective != 3) {

			// driving 
			if (keyboard_state_array[SDL_SCANCODE_W] && !keyboard_state_array[SDL_SCANCODE_SPACE]) {
				vehicle->drive(1);
			}
			else if (keyboard_state_array[SDL_SCANCODE_S] && !keyboard_state_array[SDL_SCANCODE_SPACE]) {
				vehicle->drive(-1);
			}
			if (keyboard_state_array[SDL_SCANCODE_SPACE]) {
				vehicle->handBrake();
			}
			if (!keyboard_state_array[SDL_SCANCODE_W] && !keyboard_state_array[SDL_SCANCODE_S] && !keyboard_state_array[SDL_SCANCODE_SPACE]) {
				vehicle->notGasing();
			}

			// steering
			if (keyboard_state_array[SDL_SCANCODE_D]) {
				vehicle->steerLeft(deltaTime);
			}
			else if (keyboard_state_array[SDL_SCANCODE_A]) {
				vehicle->steerRight(deltaTime);
			}
			else {
				vehicle->steerNeutral();
			}
		}

		// Different perspectives
		if (keyboard_state_array[SDL_SCANCODE_1]) {
			perspective = 1;
			camOrientation = glm::vec3(0, 1, 0);
		}
		if (keyboard_state_array[SDL_SCANCODE_2]) {
			perspective = 2;
			camOrientation = glm::vec3(0, 1, 0);
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
			camOffset = glm::vec3(11 * vehicle->getOrientation().x, 3, 11 * vehicle->getOrientation().z); //offset 20. Height 5
			// Interpolation on camdirection and position which creates a delay. More smooth camera movement. More immersive
			camDirection = camPosition + (vehicle->getPosition() - camPosition) * 0.5f;
			camPosition = camPosition + (camOffset + vehicle->getPosition() - camPosition) * 0.05f;
		}

		// Perspective 2 => for reversing
		else if (perspective == 2) {
			camOffset = glm::vec3(15 * vehicle->getOrientation().x * -1, 3, 15 * vehicle->getOrientation().z * -1); //offset 20. Height 5
			// Interpolation on camdirection and position which creates a delay. More smooth camera movement. More immersive
			camDirection = vehicle->getPosition();
			camPosition = camOffset + vehicle->getPosition();
		}

		// Perspective 3 => static point for the camera, located in the corner of the map
		else if (perspective == 3) {
			if ((buttons & SDL_BUTTON_RMASK) == SDL_BUTTON_RMASK) {
				//SDL_Log("Mouse cursor is at %d, %d", x, y); //window is 800 x 600
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
		physics->dynamicsWorld->stepSimulation(deltaTime, 1);
	}

	void adjustCamPosition() {
		handler->setCamPosition(camPosition);
		handler->setCamDirection(camDirection);
		handler->setCamOrientation(camOrientation);
	}

	// Different types of conversions between bullet and glm

	btVector3 Game::glmToBullet(const glm::vec3& vec)
	{
		return { vec.x, vec.y, vec.z };
	}
	btQuaternion Game::glmToBullet(const glm::quat& q)
	{
		return btQuaternion(q.x, q.y, q.z, q.w);
	}
	glm::vec3 Game::bulletToGlm(const btVector3& v)
	{
		return glm::vec3(v.getX(), v.getY(), v.getZ());
	}
	glm::quat Game::bulletToGlm(const btQuaternion& q)
	{
		return glm::quat(q.getW(), q.getX(), q.getY(), q.getZ());
	}

	void Game::drawDebug() {
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)1920 / (float)1080, 0.1f, 100.0f);
		glm::mat4 view = glm::lookAt(camPosition, camDirection, camOrientation);

		glUseProgram(handler->getDebugID());
		GLint MatrixID = glGetUniformLocation(handler->getDebugID(), "MVP"); // use the MVP in the simple shader
		// make the View and  Projection matrix
		glm::mat4 VP = projection * view;  // Remember order seems backwards
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &VP[0][0]);



		debugDrawer->doDebugDraw();
	}
}