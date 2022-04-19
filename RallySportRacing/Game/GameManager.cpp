#include "GameManager.h"
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
#include "Utils/GameTimer.h"
#include "Rendering/ParticleSystem.h"

#include <imgui.h>
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <Utils/Random.h>

#include "Audio/audio.h"

using namespace std;
using namespace Utils;

namespace Game {

	Random random;

	GameObject* car1;
	GameObject* environment;
	GameObject* environment2;
	GameObject* wall;
	GameObject* wall2;
	GameObject* test1;
	vector<GameObject*> gameObjects;

	Vehicle* vehicle;

	//Textures
	unsigned int smokeTexture;
	unsigned int explosionTexture;
	unsigned int blueexplosionTexture;

	//ParticleSystems
	Rendering::ParticleSystem smokeParticlesObject;
	Rendering::ParticleSystem* smokeParticlesPointer;

	Rendering::ParticleSystem explosionParticlesObject;
	Rendering::ParticleSystem* explosionParticlesPointer;

	Rendering::ParticleSystem blueexplosionParticlesObject;
	Rendering::ParticleSystem* blueexplosionParticlesPointer;

	//Debug GameObject
	GameObject* debugEnvironment;

	Rendering::SDLWindowHandler* handler;
	GameTimer* gameTimer;	
	Physics* physics;

	Audio* sound;
	int volumeButtonDelay = 0;
	bool honk = false;

	//Colors to select from when creating a model
	glm::vec3 red = glm::vec3(1.0f, 0.f, 0.f);
	glm::vec3 blue = glm::vec3(0.f, 0.f, 1.f);
	glm::vec3 green = glm::vec3(0.f, 1.f, 0.f);


	DebugDraw* debugDrawer;

	Rendering::SDLWindowHandler* Game::getHandler()
	{
		return handler;
	}

	void setupGame(Rendering::SDLWindowHandler* windowHandler) {
		physics = new Physics();
		gameTimer = GameTimer::Instance();
		handler = windowHandler;

		// Initialize audio
		sound = new Audio();

		//Load and add smokeParticles to particle render list.
		smokeTexture = handler->loadTexture("../Textures/smokeTexture.png");
		smokeParticlesObject = Rendering::ParticleSystem(1000000, smokeTexture);
		smokeParticlesPointer = &smokeParticlesObject;
		handler->addParticleSystem(smokeParticlesPointer);

		//Load and add testExplosionBlue to particle render list.
		blueexplosionTexture = handler->loadTexture("../Textures/blueExplosion.png");
		blueexplosionParticlesObject = Rendering::ParticleSystem(1000000, blueexplosionTexture);
		blueexplosionParticlesPointer = &blueexplosionParticlesObject;
		handler->addParticleSystem(blueexplosionParticlesPointer);

		//Load and add testExplosion to particle render list.
		explosionTexture = handler->loadTexture("../Textures/explosion.png");
		explosionParticlesObject = Rendering::ParticleSystem(1000000, explosionTexture);
		explosionParticlesPointer = &explosionParticlesObject;
		handler->addParticleSystem(explosionParticlesPointer);

		// test environment finished track
		Rendering::Model* test = Rendering::Model::loadModel("../Models/TerrainCollisionShape2.gltf", true);
		windowHandler->addModel(test);
		test1 = new GameObject(test, true, 2.5f, physics->dynamicsWorld); // test
		gameObjects.push_back(test1);
		test1->setInitialPosition(btVector3(-700, -90, 0));

		// test wall
		Rendering::Model* wallModel = Rendering::Model::loadModel("../Models/Wall.gltf", false);
		windowHandler->addModel(wallModel);
		wall = new GameObject(wallModel, physics->dynamicsWorld);
		gameObjects.push_back(wall);
		wall->setInitialPosition(btVector3(-70, 4, 0));
		wall->setInitialRotation(btQuaternion(0,0,1,1));

		// player vehicle, use setInitialpos to change position when starting the game
		Rendering::Model* carModel1 = Rendering::Model::loadModel("../Models/PorscheGT3_wWheels.gltf", false);
		windowHandler->addModel(carModel1);
		vehicle = new Vehicle(carModel1, physics->dynamicsWorld);
		gameObjects.push_back(vehicle);
		//vehicle->setInitialPosition(btVector3(-40, -100, 0));

		// Create player sound source
		sound->createSoundSource("self", make_tuple(vehicle->getPosition().x, vehicle->getPosition().y, vehicle->getPosition().z));

		debugDrawer = new DebugDraw();


		// Multiplayer setup
		Networking::setupNetwork(vehicle, windowHandler);

		// environment 1 test
		/*Rendering::Model* environmentModel = Rendering::Model::loadModel("../Models/SimpleEnvironment.gltf", true); // use false if not terrain
		windowHandler->addModel(environmentModel);
		environment = new GameObject(environmentModel, true, 4.0f, physics->dynamicsWorld); // use false if not terrain or use another constructor
		gameObjects.push_back(environment);
		environment->setInitialPosition(btVector3(0, 0, 0));*/

		// environment 2 test
		/*Rendering::Model* environmentModel2 = Rendering::Model::loadModel("../Models/SimpleEnvironment.gltf", true);
		windowHandler->addModel(environmentModel2);
		environment2 = new GameObject(environmentModel2, true, 1.0f, physics->dynamicsWorld);
		gameObjects.push_back(environment2);
		environment2->setInitialPosition(btVector3(-50, 0, 0));*/

		//Light Debugging Environment
		/*Rendering::Model* debugEnvironmentModel = Rendering::Model::loadModel("../Models/LightTestEnvironment.gltf", false); //
		windowHandler->addModel(debugEnvironmentModel);
		debugEnvironment = new GameObject(debugEnvironmentModel, physics->dynamicsWorld);
		gameObjects.push_back(debugEnvironment);
		debugEnvironment->setInitialPosition(btVector3(-200, 0, 0));*/
	}

	bool toScreen = true;
	bool firstTime = true;

	const Uint8* keyboard_state_array = SDL_GetKeyboardState(NULL);

	glm::vec3 directionVector;
	float speed = 10;

	glm::vec3 camPosition;
	glm::vec3 camDirection;
	glm::vec3 camOrientation;
	glm::vec3 camOffsetVector;
	glm::vec3 camOffset;

	int perspective = 1; // 1 = normal thirdperson, 2 = reverse, 3 = from a static camera above in a corner
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

	bool toggleFire = false;
	bool isOn = false;
	bool canPress = true;
	float time;

	void update() {

		// debug drawing, takes a lot of performance
		//physics->dynamicsWorld->setDebugDrawer(debugDrawer);
		//physics->dynamicsWorld->debugDrawWorld(); 

		// updates position and orientation of all gameObjects
		for (int i = 0; i < gameObjects.size(); i++) {
			gameObjects[i]->updateTransform();
		}


		// Calculate deltaTime
		if (firstTime) {

			camOrientation = glm::vec3(0, 1, 0);
			firstTime = false;
			gameTimer->startGameTime();
		}
		gameTimer->updateGameTime();

		// TODO:
		// gonna fix the code so it is more simple and clear?
		SDL_PumpEvents();
		buttons = SDL_GetMouseState(&x, &y);

		// Car movement
		if (((buttons & SDL_BUTTON_RMASK) != SDL_BUTTON_RMASK) || perspective != 3) {

			if (canPress) {
				if (keyboard_state_array[SDL_SCANCODE_E] && isOn == false) {
					isOn = true;
					//sound->engineStart(true);
					canPress = false;
					time = 0;
				}
				else if (keyboard_state_array[SDL_SCANCODE_E] && isOn == true && vehicle->getSpeed() < abs(3)) {
					isOn = false;
					//sound->engineOff();
					canPress = false;
					time = 0;
				}

			}
			time += gameTimer->getDeltaTime();
			if (time >= 1) {
				canPress = true;
			}

			if (isOn == true) {
				// engine sound
				//sound->engine(vehicle->getSpeed());

				// driving 
				if (keyboard_state_array[SDL_SCANCODE_W] && !keyboard_state_array[SDL_SCANCODE_SPACE]) {
					vehicle->drive(1);
					glm::vec3 smokeOffset = glm::vec3(2 * vehicle->getOrientation().x, vehicle->getOrientation().y + 0.34, 2 * vehicle->getOrientation().z);
					smokeParticlesObject.emitParticle(vehicle->getPosition() + smokeOffset, glm::vec3(1 * random.Float(), 1 * random.Float(), 1 * random.Float() * vehicle->getOrientation().z), 3);
					toggleFire = true;
				}
				else if (keyboard_state_array[SDL_SCANCODE_S] && !keyboard_state_array[SDL_SCANCODE_SPACE]) {
					vehicle->drive(-1);
				}
				if (keyboard_state_array[SDL_SCANCODE_SPACE]) {
					vehicle->handBrake();
				}
				if (!keyboard_state_array[SDL_SCANCODE_W] && !keyboard_state_array[SDL_SCANCODE_S] && !keyboard_state_array[SDL_SCANCODE_SPACE]) {
					vehicle->notGasing();
					glm::vec3 smokeOffset = glm::vec3(2 * vehicle->getOrientation().x, vehicle->getOrientation().y + 0.34, 2 * vehicle->getOrientation().z);
					smokeParticlesObject.emitParticle(vehicle->getPosition() + smokeOffset, glm::vec3(1 * random.Float(), 1 * random.Float(), 1 * random.Float() * vehicle->getOrientation().z), 3); // only z axis. Simulate wind effect on the smoke
					if (vehicle->getSpeed() >= 100 && toggleFire == true) {
						// spela upp ljud explosion
						
						for (int i = 0; i < 200; i++) {
							glm::vec3 smokeOffset = glm::vec3(2 * vehicle->getOrientation().x, 0.23f, 2 * vehicle->getOrientation().z);
							explosionParticlesObject.emitParticle(vehicle->getPosition() + smokeOffset, glm::vec3(0.3 * random.Float() * vehicle->getOrientation().x, 0.3 * random.Float(), 0.3 * random.Float() * vehicle->getOrientation().z), 0.05f);
							blueexplosionParticlesObject.emitParticle(vehicle->getPosition() + smokeOffset, glm::vec3(0.01 * random.Float() * vehicle->getOrientation().x, 0.01 * random.Float(), 0.01 * random.Float() * vehicle->getOrientation().z), 0.05f);
						}
						toggleFire = false;
					}
				}
				// steering
				if (keyboard_state_array[SDL_SCANCODE_D]) {
					vehicle->steerLeft(gameTimer->getDeltaTime());
				}
				else if (keyboard_state_array[SDL_SCANCODE_A]) {
					vehicle->steerRight(gameTimer->getDeltaTime());
				}
				else {
					vehicle->steerNeutral();
				}

			}
			else  {
				vehicle->steerNeutral();
				vehicle->handBrake();
			}
			
		}

		// Different perspectives
		if (keyboard_state_array[SDL_SCANCODE_1] || (buttons & SDL_BUTTON_LMASK) == SDL_BUTTON_LMASK) {
			perspective = 1;
			camOrientation = glm::vec3(0, 1, 0);
		}
		if (keyboard_state_array[SDL_SCANCODE_C] || (buttons & SDL_BUTTON_RMASK) == SDL_BUTTON_RMASK) {
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
			camPosition = camPosition + (camOffset + vehicle->getPosition() - camPosition) * 0.1f;
		}

		// Perspective 2 => for reversing
		else if (perspective == 2) {
			camOffset = glm::vec3(15 * vehicle->getOrientation().x * -1, 3, 15 * vehicle->getOrientation().z * -1); //offset 20. Height 5

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
				horizontalAngle += mouseSpeed * gameTimer->getDeltaTime() * float(WIDTH / 2 - x); // widht and height of window
				verticalAngle += mouseSpeed * gameTimer->getDeltaTime() * float(HEIGHT / 2 - y);
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
					camPosition += direction * gameTimer->getDeltaTime()  * cameraSpeed;
				}

				if (keyboard_state_array[SDL_SCANCODE_S]) {
					camPosition -= direction * gameTimer->getDeltaTime() * cameraSpeed;
				}

				if (keyboard_state_array[SDL_SCANCODE_D]) {
					camPosition += right * gameTimer->getDeltaTime() * cameraSpeed;
				}

				if (keyboard_state_array[SDL_SCANCODE_A]) {
					camPosition -= right * gameTimer->getDeltaTime()  * cameraSpeed;
				}

				if (keyboard_state_array[SDL_SCANCODE_LSHIFT]) {
					cameraSpeed = 100.0f;
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
		physics->dynamicsWorld->stepSimulation(gameTimer->getDeltaTime(), 1);

		// horn sound
		if (keyboard_state_array[SDL_SCANCODE_H]) {
			honk = true;
		}
		else {
			honk = false;
		}

		// volume change
		if (volumeButtonDelay > 0) {
			volumeButtonDelay--;
		}
		if (keyboard_state_array[SDL_SCANCODE_O] && volumeButtonDelay == 0) {
			sound->volumeUp();
			volumeButtonDelay = 20;
		}
		if (keyboard_state_array[SDL_SCANCODE_L] && volumeButtonDelay == 0) {
			sound->volumeDown();
			volumeButtonDelay = 20;
		}

		// Update "self" sound source
		sound->updateSoundSource("self", make_tuple(vehicle->getPosition().x, vehicle->getPosition().y, vehicle->getPosition().z), vehicle->getSpeed(), honk);
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

	void Game::applicationExit() {
		Networking::terminateNetwork();
	}

}