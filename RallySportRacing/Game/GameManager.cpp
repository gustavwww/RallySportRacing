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
#include "Vehicle.h"

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
#define PI 3.1415926538f

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
	GameObject* light;
	GameObject* test21;
	GameObject* test31;
	vector<GameObject*> gameObjects;

	Vehicle* vehicle;

	//Textures
	unsigned int smokeTexture;
	unsigned int explosionTexture;
	unsigned int blueexplosionTexture;
	unsigned int tireTrackTexture;
	unsigned int tireTrack2Texture;
	unsigned int dirtTexture;
	unsigned int rainTexture;
	unsigned int snowTexture;
	unsigned int terrainParticleTexture;
	unsigned int terrainParticleTexture2;

	//ParticleSystems
	Rendering::ParticleSystem smokeParticlesObject;
	Rendering::ParticleSystem* smokeParticlesPointer;

	Rendering::ParticleSystem explosionParticlesObject;
	Rendering::ParticleSystem* explosionParticlesPointer;

	Rendering::ParticleSystem blueexplosionParticlesObject;
	Rendering::ParticleSystem* blueexplosionParticlesPointer;

	Rendering::ParticleSystem tireTrackParticlesObject;
	Rendering::ParticleSystem* tireTrackParticlesPointer;

	Rendering::ParticleSystem tireTrack2ParticlesObject;
	Rendering::ParticleSystem* tireTrack2ParticlesPointer;

	Rendering::ParticleSystem dirtParticlesObject;
	Rendering::ParticleSystem* dirtParticlesPointer;

	Rendering::ParticleSystem rainParticlesObject;
	Rendering::ParticleSystem* rainParticlesPointer;

	Rendering::ParticleSystem snowParticlesObject;
	Rendering::ParticleSystem* snowParticlesPointer;

	Rendering::ParticleSystem terrainParticleObject;
	Rendering::ParticleSystem* terrainParticlePointer;

	Rendering::ParticleSystem terrainParticleObject2;
	Rendering::ParticleSystem* terrainParticlePointer2;

	//Debug GameObject
	GameObject* debugEnvironment;

	Rendering::SDLWindowHandler* handler;
	GameTimer* gameTimer;	
	Physics* physics;

	Audio* sound;
	int volumeButtonDelay = 0;
	string soundString = "00000";

	//Colors to select from when creating a model
	glm::vec3 red = glm::vec3(1.0f, 0.f, 0.f);
	glm::vec3 blue = glm::vec3(0.f, 0.f, 1.f);
	glm::vec3 green = glm::vec3(0.f, 1.f, 0.f);


	// for checkpoints
	// collisionobjects one for every checkpoint
	btCollisionObject* checkpoint0; // this is the spawn point
	btCollisionObject* checkpoint1; // needs one of these for every checkpoint
	btCollisionObject* checkpoint2;
	btCollisionObject* checkpoint3;
	btCollisionObject* checkpoint4;
	btCollisionObject* checkpoint5;
	btCollisionObject* checkpoint6;
	btCollisionObject* checkpoint7;
	//btCollisionObject* checkpoint8;
	//btCollisionObject* checkpoint9; // could be end point or we just have the start point as the endpoint
	vector<btCollisionObject*> checkpoints; // list of all checkpoints

	// transforms one for every checkpoint
	btTransform transform0; // this is the spawn point
	btTransform transform1; // needs one of these for every checkpoint. Start point for the race
	btTransform transform2;
	btTransform transform3;
	btTransform transform4;
	btTransform transform5;
	btTransform transform6;
	btTransform transform7;
	//btTransform transform8;
	//btTransform transform9;
	vector<btTransform> transforms; // list of all transforms for checkpoints

	btCollisionObject* latestReachedCheckpoint;
	btCollisionShape* checkPointShape; // same for all checkpoints

	DebugDraw* debugDrawer;

	Rendering::SDLWindowHandler* Game::getHandler()
	{
		return handler;
	}
	void initCheckPoints() {

		checkPointShape = new btBoxShape(btVector3(6, 4, 6));// same for all checkpoints

		transform0.setIdentity(); // initilizes the different transforms
		transform1.setIdentity(); // initilizes the different transforms
		transform2.setIdentity(); // initilizes the different transforms
		transform3.setIdentity(); // initilizes the different transforms
		transform4.setIdentity(); // initilizes the different transforms
		transform5.setIdentity(); // initilizes the different transforms
		transform6.setIdentity(); // initilizes the different transforms
		transform7.setIdentity(); // initilizes the different transforms
		//transform8.setIdentity(); // initilizes the different transforms
		//transform9.setIdentity(); // initilizes the different transforms
	 
		transform0.setOrigin(btVector3(-1432.38, 38.5744, -474.198)); // hardcoded values for the position for the specific checkpoint
		transform0.setRotation(btQuaternion(btVector3(0, 1, 0), -1.125 *PI));
		transform1.setOrigin(btVector3(-1277.25, 35.2272, -1105.91)); // hardcoded values for the position for the specific checkpoint
		transform1.setRotation(btQuaternion(btVector3(0, 1, 0), -1.2 * PI));
		transform2.setOrigin(btVector3(-645.366, 24.9702, -1107.97)); // hardcoded values for the position for the specific checkpoint
		transform2.setRotation(btQuaternion(btVector3(0, 1, 0), 2 * PI /4));
		transform3.setOrigin(btVector3(25.3112, -4.35853, 42.0755)); // hardcoded values for the position for the specific checkpoint
		transform4.setOrigin(btVector3(-216.312, -3.24275, 477.574)); // hardcoded values for the position for the specific checkpoint
		transform4.setRotation(btQuaternion(btVector3(0, 1, 0), 2*PI/2)); // temporary last checkpoint, untill the models gets finished
		transform5.setOrigin(btVector3(-364.28, 10.8325, -545.443)); // hardcoded values for the position for the specific checkpoint
		transform5.setRotation(btQuaternion(btVector3(0, 1, 0), 2*PI * 0.57));
		transform6.setOrigin(btVector3(-1134.75, 36.4135, -741.386)); // hardcoded values for the position for the specific checkpoint
		transform6.setRotation(btQuaternion(btVector3(0, 1, 0), -PI/4));
		transform7.setOrigin(btVector3(-1403.99, 38.0386, -416.225)); // hardcoded values for the position for the specific checkpoint
		transform7.setRotation(btQuaternion(btVector3(0, 1, 0), -PI / 2));
		//transform8.setOrigin(btVector3(-248, 8, -768)); // hardcoded values for the position for the specific checkpoint
		//transform8.setRotation(btQuaternion(btVector3(0, 1, 0), 3*PI/5));
		//transform9.setOrigin(btVector3(-126.6, 1, -307)); // hardcoded values for the position for the specific checkpoint
		//transform9.setRotation(btQuaternion(btVector3(0, 1, 0), PI/11));
		 
		// adds all checkpoints objects to list
		checkpoints.push_back(checkpoint0);
		checkpoints.push_back(checkpoint1);
		checkpoints.push_back(checkpoint2);
		checkpoints.push_back(checkpoint3);
		checkpoints.push_back(checkpoint4);
		checkpoints.push_back(checkpoint5);
		checkpoints.push_back(checkpoint6);
		checkpoints.push_back(checkpoint7);
		//checkpoints.push_back(checkpoint8);
		//checkpoints.push_back(checkpoint9);

		// adds all checkpoints transforms to list
		transforms.push_back(transform0);
		transforms.push_back(transform1);
		transforms.push_back(transform2);
		transforms.push_back(transform3);
		transforms.push_back(transform4);
		transforms.push_back(transform5);
		transforms.push_back(transform6);
		transforms.push_back(transform7);
		//transforms.push_back(transform8);
		//transforms.push_back(transform9);


		for (int i = 0; i < checkpoints.size(); i++) {
			checkpoints[i] = new btCollisionObject();
			checkpoints[i]->setCollisionShape(checkPointShape);

			checkpoints[i]->setWorldTransform(transforms[i]);
			checkpoints[i]->setCollisionFlags(checkpoints[i]->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE); // it can detect collision but it does not do anything besides that

			physics->dynamicsWorld->addCollisionObject(checkpoints[i]);
		}

		latestReachedCheckpoint = checkpoints[0]; // sets the latestcheckpointreached to the first checkpoint, 0 is spawn point, 1 is start checkpoint 
	}

	double dirtFriction = 2.0f; // friction of dirtroad
	double terrainFriction = 1.9f; // friction of other terrain
	double highwayFriction = 2.5f; // friction of other terrain

	void setupGame(Rendering::SDLWindowHandler* windowHandler) {
		physics = new Physics();
		gameTimer = GameTimer::Instance();
		handler = windowHandler;

		// Initialize audio
		sound = Audio::Instance();

		//Load and add dirtParticles to particle render list.
		dirtTexture = handler->loadTexture("../Textures/dirtTexture.png");
		dirtParticlesObject = Rendering::ParticleSystem(1000000, dirtTexture);
		tireTrackParticlesPointer = &dirtParticlesObject;
		handler->addParticleSystem(tireTrackParticlesPointer);

		//Load and add testExplosion to particle render list.
		explosionTexture = handler->loadTexture("../Textures/explosion.png");
		explosionParticlesObject = Rendering::ParticleSystem(1000000, explosionTexture);
		explosionParticlesPointer = &explosionParticlesObject;
		handler->addParticleSystem(explosionParticlesPointer);

		//Load and add testExplosionBlue to particle render list.
		blueexplosionTexture = handler->loadTexture("../Textures/blueExplosion.png");
		blueexplosionParticlesObject = Rendering::ParticleSystem(1000000, blueexplosionTexture);
		blueexplosionParticlesPointer = &blueexplosionParticlesObject;
		handler->addParticleSystem(blueexplosionParticlesPointer);

		//Load and add tireTrack to particle render list.
		tireTrackTexture = handler->loadTexture("../Textures/testTrack.png");
		tireTrackParticlesObject = Rendering::ParticleSystem(1000000, tireTrackTexture);
		tireTrackParticlesPointer = &tireTrackParticlesObject;
		handler->addParticleSystem(tireTrackParticlesPointer);

		//Load and add tireTrack to particle render list.
		tireTrack2Texture = handler->loadTexture("../Textures/testTrack2.png");
		tireTrack2ParticlesObject = Rendering::ParticleSystem(1000000, tireTrack2Texture);
		tireTrack2ParticlesPointer = &tireTrack2ParticlesObject;
		handler->addParticleSystem(tireTrack2ParticlesPointer);

		//Load and add smokeParticles to particle render list.
		smokeTexture = handler->loadTexture("../Textures/smokeTexture.png");
		smokeParticlesObject = Rendering::ParticleSystem(1000000, smokeTexture);
		smokeParticlesPointer = &smokeParticlesObject;
		handler->addParticleSystem(smokeParticlesPointer);

		//Load and add rainTexture to particle render list.
		rainTexture = handler->loadTexture("../Textures/rainTexture.png");
		rainParticlesObject = Rendering::ParticleSystem(1000000, rainTexture);
		rainParticlesPointer = &rainParticlesObject;
		handler->addParticleSystem(rainParticlesPointer);

		//Load and add snowTexture to particle render list.
		snowTexture = handler->loadTexture("../Textures/snowTexture.png");
		snowParticlesObject = Rendering::ParticleSystem(1000000, snowTexture);
		snowParticlesPointer = &snowParticlesObject;
		handler->addParticleSystem(snowParticlesPointer);

		//Load and add terrainParticletexture to particle render list.
		terrainParticleTexture = handler->loadTexture("../Textures/terrainParticleTexture.png");
		terrainParticleObject = Rendering::ParticleSystem(1000000, terrainParticleTexture);
		terrainParticlePointer = &terrainParticleObject;
		handler->addParticleSystem(terrainParticlePointer);

		//Load and add terrainParticletexture to particle render list.
		terrainParticleTexture2 = handler->loadTexture("../Textures/terrainParticleTexture2.png");
		terrainParticleObject2 = Rendering::ParticleSystem(1000000, terrainParticleTexture2);
		terrainParticlePointer2 = &terrainParticleObject2;
		handler->addParticleSystem(terrainParticlePointer2);

		// model loading

		//////////////////////////////////////////////////////////////////////////////////////////////////////

		// test environment finished track
		Rendering::Model* test = Rendering::Model::loadModel("../Models/EnvironmentRallyTrack.gltf", true, false);
		windowHandler->addModel(test);
		test1 = new GameObject(test, true, terrainFriction, physics->dynamicsWorld); // test
		gameObjects.push_back(test1);
		test1->setInitialPosition(btVector3(-700, -90, 0));

		
		// test environment finished track
		Rendering::Model* test2 = Rendering::Model::loadModel("../Models/HighwayModel.gltf", true, false);
		windowHandler->addModel(test2);
		test21 = new GameObject(test2, true, highwayFriction, physics->dynamicsWorld); // test
		gameObjects.push_back(test21);
		test21->setInitialPosition(btVector3(-700, -90, 0));

		// test environment finished track
		Rendering::Model* test3 = Rendering::Model::loadModel("../Models/DirtRoadModel.gltf", true, false);
		windowHandler->addModel(test3);
		test31 = new GameObject(test3, true, dirtFriction, physics->dynamicsWorld); // test
		gameObjects.push_back(test31);
		test31->setInitialPosition(btVector3(-700, -90, 0));

		//////////////////////////////////////////////////////////////////////////////////////////////////////

		// test wall
		Rendering::Model* wallModel = Rendering::Model::loadModel("../Models/test.gltf", false, false);
		windowHandler->addModel(wallModel);
		wall = new GameObject(wallModel, physics->dynamicsWorld);
		gameObjects.push_back(wall);
		wall->setInitialPosition(btVector3(-70, 0, 0));
		//wall->setInitialRotation(btQuaternion(0,0,1,1));

		// test light
		Rendering::Model* lightModel = Rendering::Model::loadModel("../Models/light.gltf", false, true);
		windowHandler->addModel(lightModel);
		light = new GameObject(lightModel, physics->dynamicsWorld);
		gameObjects.push_back(light);
		light->setInitialPosition(btVector3(-70, 4, 0));

		// player vehicle, use setInitialpos to change position when starting the game
		Rendering::Model* carModel1 = Rendering::Model::loadModel("../Models/PorscheGT3.gltf", false, true);
		windowHandler->addModel(carModel1);
		vehicle = new Vehicle(carModel1, physics->dynamicsWorld);
		gameObjects.push_back(vehicle);
		vehicle->setInitialPosition(btVector3(-1438.15, 39.6246+1, -458.328));
		vehicle->vehicle->getRigidBody()->getWorldTransform().setRotation(btQuaternion(btVector3(0, 1, 0), -13*PI/12));

		// Create player sound source
		sound->createSoundSource(0, vehicle->getPosition());

		debugDrawer = new DebugDraw();

		initCheckPoints();

		// Multiplayer setup
		Networking::setupNetwork("Gustav", vehicle, windowHandler);

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

	// temp names for adjusting camera distance when in perspective 1
	double cameraDistance = 11;
	double cameraDistance2 = 3;

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

	bool pressedW = false;
	// for engine off and on
	bool isOn = false;
	bool canPress = true;
	float time;
	bool engineOnOffToggle = true;
	float engineDelay;

	// test for multiple random backfires
	bool temp1 = false;
	float temp1delay;
	bool temp1toggle = false;

	// delay for turning the car back up
	bool resetCarToggle = true;
	float resetCarDelay;

	// delay for completely resetting the vehicle, could be used for check points 
	bool completeResetCarToggle = true;
	float completeResetCarDelay;

	// delay for backfire
	bool backFireToggle = true;
	float backFireDelay;

	// timer for gas when enginestarts
	double gasTimer = 3;

	bool everyOtherLoop = true;

	bool toggleRain = false;
	bool toggleSnow = false;

	// variables for starting the race and handling race
	float raceCountDown = 3;
	bool isCountingDown = false;
	float raceTime = 0;
	bool timingRace = false;
	int checkpointsReached = 0;

	void activateRain() {
		glm::vec3 rainOffset1 = glm::vec3(25 * random.Float(), 25 * random.Float(), 25 * random.Float());
		rainParticlesObject.emitParticle(vehicle->getPosition() + rainOffset1, glm::vec3(0, -9, 0), 2, 2);

		glm::vec3 rainOffset2 = glm::vec3(-25 * random.Float(), 25 * random.Float(), -25 * random.Float());
		rainParticlesObject.emitParticle(vehicle->getPosition() + rainOffset2, glm::vec3(0, -9, 0), 2, 2);

		glm::vec3 rainOffset3 = glm::vec3(-25 * random.Float(), 25 * random.Float(), 25 * random.Float());
		rainParticlesObject.emitParticle(vehicle->getPosition() + rainOffset3, glm::vec3(0, -9, 0), 2, 2);

		glm::vec3 rainOffset4 = glm::vec3(25 * random.Float(), 25 * random.Float(), -25 * random.Float());
		rainParticlesObject.emitParticle(vehicle->getPosition() + rainOffset4, glm::vec3(0, -9, 0), 2, 2);
	}

	void activateSnow() {
		glm::vec3 rainOffset1 = glm::vec3(25 * random.Float(), 25 * random.Float(), 25 * random.Float());
		snowParticlesObject.emitParticle(vehicle->getPosition() + rainOffset1, glm::vec3(0, -2, 0), 4, 3);

		glm::vec3 rainOffset2 = glm::vec3(-25 * random.Float(), 25 * random.Float(), -25 * random.Float());
		snowParticlesObject.emitParticle(vehicle->getPosition() + rainOffset2, glm::vec3(0, -2, 0), 4, 3);

		glm::vec3 rainOffset3 = glm::vec3(-25 * random.Float(), 25 * random.Float(), 25 * random.Float());
		snowParticlesObject.emitParticle(vehicle->getPosition() + rainOffset3, glm::vec3(0, -2, 0), 4, 3);

		glm::vec3 rainOffset4 = glm::vec3(25 * random.Float(), 25 * random.Float(), -25 * random.Float());
		snowParticlesObject.emitParticle(vehicle->getPosition() + rainOffset4, glm::vec3(0, -2, 0), 4, 3);
	}

	float Game::getRaceTime()
	{
		return raceTime;
	}
	float Game::getCountDownTime()
	{
		return raceCountDown;
	}

	float Game::getVehicleSpeed()
	{
		return vehicle->getSpeed();
	}

	bool Game::getCountDown()
	{
		return isCountingDown;
	}

	int Game::getCheckpointsReached()
	{
		return checkpointsReached;
	}

	void Game::setTextureIndex(int a) {
		for (int i = 0; i < gameObjects.size(); i++) {
			//gameObjects[i]->getModel()->updateMaterial(a, "chassiColour");
		}
	}

	void checkCollisions() {
		// Collisionhandling between a pair of objects
		// Bullet physics does not give a predefined check collision with method
		int numManifolds = physics->dynamicsWorld->getDispatcher()->getNumManifolds();
		for (int i = 0; i < numManifolds; i++)
		{
			btPersistentManifold* contactManifold = physics->dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
			const btCollisionObject* obA = contactManifold->getBody0();
			const btCollisionObject* obB = contactManifold->getBody1();

			int numContacts = contactManifold->getNumContacts();
			for (int j = 0; j < numContacts; j++)
			{
				btManifoldPoint& pt = contactManifold->getContactPoint(j);
				if (pt.getDistance() < 0.f)
				{

					// if we hit the last checkpoint (finishline) (which currently is the start checkpoint) and have touched the checkpoint before. Then we have finished the race. Racetimer stops counting
					// could change so that it also checks a variable if we have hit all checkpoints. Now we can just go back and forward to complete the race
					if ((obA->getWorldArrayIndex() == vehicle->vehicle->getRigidBody()->getWorldArrayIndex() && obB->getWorldArrayIndex() == checkpoints[0]->getWorldArrayIndex()) && (checkpointsReached == (checkpoints.size()-1)) && timingRace) {
						timingRace = false;
						checkpointsReached++;
						//cout << checkpointsReached << endl;
						//cout << roundf(raceTime) << endl;
						// do something here, Like show the timer or put it in a leaderboard
						// raceTime is the timer for the race
						Networking::sendTime(raceTime);
						// play sound here xxxxxxxxxx
					}

					for (int i = 0; i < checkpoints.size(); i++) { // checks for collision between any checkpoint and the vehicle
						if (i != 0) {
							if (obA->getWorldArrayIndex() == vehicle->vehicle->getRigidBody()->getWorldArrayIndex() && obB->getWorldArrayIndex() == checkpoints[i]->getWorldArrayIndex() && latestReachedCheckpoint == checkpoints[i - 1] && timingRace) {
								//cout << "Kollision med: " << checkpoints[i]->getWorldArrayIndex() << endl;
								latestReachedCheckpoint = checkpoints[i];
								checkpointsReached++;
								//cout << checkpointsReached << endl;
								// play sound here xxxxxxxxxxxxx
							}
						}
					}
				}
			}
		}
	}

	void update() {

		// debug drawing, takes a lot of performance
		/*physics->dynamicsWorld->setDebugDrawer(debugDrawer);
		for (int i = 0; i < checkpoints.size(); i++) { // draws every checkpoints
			physics->dynamicsWorld->debugDrawObject(transforms[i], checkPointShape, btVector3(0, 255, 0));
		}*/
		//physics->dynamicsWorld->debugDrawWorld(); 

		// method for handling collisions with checkpoints
		checkCollisions();

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

		SDL_PumpEvents();
		buttons = SDL_GetMouseState(&x, &y);

		// weather 
		if (keyboard_state_array[SDL_SCANCODE_F1]) {
			toggleRain = false;
			toggleSnow = false;
			sound->playRainSound(false);
		}
		if (keyboard_state_array[SDL_SCANCODE_F2]) {
			toggleRain = true;
			toggleSnow = false;
			sound->playRainSound(true);
		}
		if (keyboard_state_array[SDL_SCANCODE_F3]) {
			toggleSnow = true;
			toggleRain = false;
			sound->playRainSound(false);
		}
		if (toggleRain) {
			activateRain();
		}
		if (toggleSnow) {
			activateSnow();
		}

		// Start race
		if (keyboard_state_array[SDL_SCANCODE_RETURN] && resetCarToggle) {
			perspective = 1;
			resetCarToggle = false;
			resetCarDelay = 0;
			checkpointsReached = 0;

			vehicle->vehicle->getRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
			latestReachedCheckpoint = checkpoints[0];
			vehicle->setInitialPosition(latestReachedCheckpoint->getWorldTransform().getOrigin() + btVector3(0, 0, 0));
			vehicle->setInitialRotation(latestReachedCheckpoint->getWorldTransform().getRotation());

			raceCountDown = 3;
			isCountingDown = true;
			raceTime = 0;
			sound->playStartSound();
		}
		if (isCountingDown) {
			vehicle->vehicle->getRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
			raceCountDown -= gameTimer->getDeltaTime(); // variable to show on screen. Maybe cast to int or round it
			//cout << roundf(raceCountDown) << endl; skipps 3 because it does not round. Better to use roundf i think
			raceTime = 0;
			timingRace = false;
		}
		if (raceCountDown <= 0) {
			isCountingDown = false;
			raceTime = 0;
			timingRace = true;
			raceCountDown = 3; // to stop the if 
		}
		if (timingRace) {
			raceTime += gameTimer->getDeltaTime();
			//cout << roundf(raceTime) << endl;
		}



		light->setPosition(handler->getLightPosition());

		// Car movement
		if (((buttons & SDL_BUTTON_RMASK) != SDL_BUTTON_RMASK) || perspective != 3) {

			if (vehicle->vehicle->getWheelInfo(2).m_frictionSlip == terrainFriction) { // checks one rear wheel if it is in contact with terrain
				vehicle->setMaxSpeed(80);
			}
			if (vehicle->vehicle->getWheelInfo(2).m_frictionSlip == dirtFriction) { // checks one rear wheel if it is in contact with terrain
				vehicle->setMaxSpeed(200);
			}
			if (vehicle->vehicle->getWheelInfo(2).m_frictionSlip == highwayFriction) { // checks one rear wheel if it is in contact with terrain
				vehicle->setMaxSpeed(250);
			}

			if (engineOnOffToggle) {
				if (keyboard_state_array[SDL_SCANCODE_E] && isOn == false) {
					isOn = true;
					// Play start sound
					soundString[0] = '1';
					engineOnOffToggle = false;
					engineDelay = 0;
					gasTimer = 0;
	
					for (int i = 0; i < 100; i++) {
						glm::vec3 smokeOffset = glm::vec3(2 * vehicle->getOrientation().x, vehicle->getOrientation().y + 0.34, 2 * vehicle->getOrientation().z);
						smokeParticlesObject.emitParticle(vehicle->getPosition() + smokeOffset, glm::vec3(1 * random.Float(), 1 * random.Float(), 1 * random.Float() * vehicle->getOrientation().z), 3, 0.2);
					}
				}
				else if (keyboard_state_array[SDL_SCANCODE_E] && isOn == true && vehicle->getSpeed() < abs(3)) {
					isOn = false;
					// Plays stop sound
					soundString[0] = '3';
					engineOnOffToggle = false;
					engineDelay = 0;
				}
			}

			gasTimer += gameTimer->getDeltaTime();
			if (gasTimer <= 1.5) {
				glm::vec3 smokeOffset = glm::vec3(2 * vehicle->getOrientation().x, vehicle->getOrientation().y + 0.34, 2 * vehicle->getOrientation().z);
				smokeParticlesObject.emitParticle(vehicle->getPosition() + smokeOffset, glm::vec3(1 * random.Float(), 1 * random.Float(), 1 * random.Float() * vehicle->getOrientation().z), 3, 0.2);
			}

			engineDelay += gameTimer->getDeltaTime();
			if (engineDelay >= 1) {
				engineOnOffToggle = true;
			}

			if (isOn == true) {
				
				// dirt particles
				if ((vehicle->getSpeed() <= -10 || vehicle->getSpeed() >= 10) && vehicle->vehicle->getWheelInfo(2).m_frictionSlip == dirtFriction) { // checks rearwheel if it is in contact with a model that has fricion values of dirtFriction
					
					// Change soundString terrain identifier
					soundString[3] = '1';

					glm::vec3 rearWheel1Pos = bulletToGlm(vehicle->vehicle->getWheelTransformWS(2).getOrigin());
					glm::vec3 rearWheel2Pos = bulletToGlm(vehicle->vehicle->getWheelTransformWS(3).getOrigin());
					dirtParticlesObject.emitParticle(rearWheel1Pos, glm::vec3(1 * random.Float(), 1 * random.Float(), 1 * random.Float() * vehicle->getOrientation().z), 3, 0.3);
					dirtParticlesObject.emitParticle(rearWheel2Pos, glm::vec3(1 * random.Float(), 1 * random.Float(), 1 * random.Float() * vehicle->getOrientation().z), 3, 0.3);

					glm::vec3 frontWheel1Pos = bulletToGlm(vehicle->vehicle->getWheelTransformWS(0).getOrigin());
					glm::vec3 frontWheel2Pos = bulletToGlm(vehicle->vehicle->getWheelTransformWS(1).getOrigin());
					dirtParticlesObject.emitParticle(frontWheel1Pos, glm::vec3(1 * random.Float(), 1 * random.Float(), 1 * random.Float() * vehicle->getOrientation().z), 3, 0.3);
					dirtParticlesObject.emitParticle(frontWheel2Pos, glm::vec3(1 * random.Float(), 1 * random.Float(), 1 * random.Float() * vehicle->getOrientation().z), 3, 0.3);
				}

				// terrain particles
				if ((vehicle->getSpeed() <= -10 || vehicle->getSpeed() >= 10) && vehicle->vehicle->getWheelInfo(2).m_frictionSlip == terrainFriction) { // checks rearwheel if it is in contact with a model that has fricion values of dirtFriction

					glm::vec3 rearWheel1Pos = bulletToGlm(vehicle->vehicle->getWheelTransformWS(2).getOrigin());
					glm::vec3 rearWheel2Pos = bulletToGlm(vehicle->vehicle->getWheelTransformWS(3).getOrigin());

					terrainParticleObject.emitParticle(rearWheel1Pos, glm::vec3(1 * random.Float(), 0 * random.Float(), 1 * random.Float() * vehicle->getOrientation().z), 1, 0.3);
					terrainParticleObject.emitParticle(rearWheel2Pos, glm::vec3(1 * random.Float(), 0 * random.Float(), 1 * random.Float() * vehicle->getOrientation().z), 1, 0.3);
					terrainParticleObject2.emitParticle(rearWheel1Pos, glm::vec3(1 * random.Float(), 0 * random.Float(), 1 * random.Float() * vehicle->getOrientation().z), 1, 0.3);
					terrainParticleObject2.emitParticle(rearWheel2Pos, glm::vec3(1 * random.Float(), 0 * random.Float(), 1 * random.Float() * vehicle->getOrientation().z), 1, 0.3);
				}
				if ((vehicle->getSpeed() <= -10 || vehicle->getSpeed() >= 10) && (vehicle->vehicle->getWheelInfo(2).m_frictionSlip == highwayFriction)) { // checks rearwheel if it is in contact with a model that has fricion values of dirtFriction
					glm::vec3 rearWheel1Pos = bulletToGlm(vehicle->vehicle->getWheelTransformWS(2).getOrigin());
					glm::vec3 rearWheel2Pos = bulletToGlm(vehicle->vehicle->getWheelTransformWS(3).getOrigin());

					terrainParticleObject.emitParticle(rearWheel1Pos, glm::vec3(1 * random.Float(), 0 * random.Float(), 1 * random.Float() * vehicle->getOrientation().z), 1, 0.1);
					terrainParticleObject.emitParticle(rearWheel2Pos, glm::vec3(1 * random.Float(), 0 * random.Float(), 1 * random.Float() * vehicle->getOrientation().z), 1, 0.1);
					terrainParticleObject2.emitParticle(rearWheel1Pos, glm::vec3(1 * random.Float(), 0 * random.Float(), 1 * random.Float() * vehicle->getOrientation().z), 1, 0.1);
					terrainParticleObject2.emitParticle(rearWheel2Pos, glm::vec3(1 * random.Float(), 0 * random.Float(), 1 * random.Float() * vehicle->getOrientation().z), 1, 0.1);
				}

				// Engine sound on
				if (soundString[0] == '0') {
					soundString[0] = '2';
				}
				
				float randomNumber = random.Float() * 10;
				everyOtherLoop = !everyOtherLoop;
				if ((int)randomNumber % 7 == 0 && everyOtherLoop) { // random small amount of gas coming out when the engine is on
					glm::vec3 smokeOffset = glm::vec3(2 * vehicle->getOrientation().x, vehicle->getOrientation().y + 0.34, 2 * vehicle->getOrientation().z);
					smokeParticlesObject.emitParticle(vehicle->getPosition() + smokeOffset, glm::vec3(1 * random.Float(), 1 * random.Float(), 1 * random.Float() * vehicle->getOrientation().z), 2, 0.2);
				}
				
				// driving 

				if (keyboard_state_array[SDL_SCANCODE_W] && !keyboard_state_array[SDL_SCANCODE_SPACE]) {
					vehicle->drive(1);
					pressedW = true;
					soundString[4] = '1';
				}
				else if (keyboard_state_array[SDL_SCANCODE_S] && !keyboard_state_array[SDL_SCANCODE_SPACE]) {
					vehicle->drive(-1);
					soundString[4] = '2';
				}
				if (keyboard_state_array[SDL_SCANCODE_SPACE] && vehicle->vehicle->getWheelInfo(2).m_frictionSlip == highwayFriction) {
					vehicle->handBrake();
					if (vehicle->getSpeed() >= 30) {
						glm::vec3 rearWheel1Pos = bulletToGlm(vehicle->vehicle->getWheelTransformWS(2).getOrigin());
						glm::vec3 rearWheel2Pos = bulletToGlm(vehicle->vehicle->getWheelTransformWS(3).getOrigin());

						tireTrackParticlesObject.emitParticle(rearWheel1Pos - glm::vec3(0, 0.3, 0), glm::vec3(0, -0.05, 0), 10, 0.2);
						tireTrackParticlesObject.emitParticle(rearWheel2Pos - glm::vec3(0, 0.3, 0), glm::vec3(0, -0.05, 0), 10, 0.2);
					}
				}
				if (keyboard_state_array[SDL_SCANCODE_SPACE] && (vehicle->vehicle->getWheelInfo(2).m_frictionSlip == dirtFriction || vehicle->vehicle->getWheelInfo(2).m_frictionSlip == terrainFriction)) {
					vehicle->handBrake();
					if (vehicle->getSpeed() >= 30) {
						glm::vec3 rearWheel1Pos = bulletToGlm(vehicle->vehicle->getWheelTransformWS(2).getOrigin());
						glm::vec3 rearWheel2Pos = bulletToGlm(vehicle->vehicle->getWheelTransformWS(3).getOrigin());

						tireTrack2ParticlesObject.emitParticle(rearWheel1Pos - glm::vec3(0, 0.3, 0), glm::vec3(0, -0.05, 0), 10, 0.2);
						tireTrack2ParticlesObject.emitParticle(rearWheel2Pos - glm::vec3(0, 0.3, 0), glm::vec3(0, -0.05, 0), 10, 0.2);
					}
				}
				if (!keyboard_state_array[SDL_SCANCODE_W] && !keyboard_state_array[SDL_SCANCODE_S] && !keyboard_state_array[SDL_SCANCODE_SPACE]) {
					vehicle->notGasing();
				
					if ((vehicle->getSpeed() >= 100 && pressedW == true && backFireToggle == true) || temp1) { // 169
						// spela upp ljud explosion
						temp1delay = 0;
							soundString[2] = '1';
							for (int i = 0; i < 400; i++) {
								glm::vec3 smokeOffset = glm::vec3(2 * vehicle->getOrientation().x, vehicle->getOrientation().y + 0.34, 2 * vehicle->getOrientation().z);
								explosionParticlesObject.emitParticle(vehicle->getPosition() + smokeOffset, glm::vec3(0,0,0), 0.04f, 0.1);
							}
							for (int i = 0; i < 10; i++) {
								glm::vec3 smokeOffset = glm::vec3(2 * vehicle->getOrientation().x, vehicle->getOrientation().y + 0.34, 2 * vehicle->getOrientation().z);
								smokeParticlesObject.emitParticle(vehicle->getPosition() + smokeOffset, glm::vec3(1 * random.Float(), 1 * random.Float(), 1 * random.Float() * vehicle->getOrientation().z), 0.5, 0.2);
							}
							pressedW = false;
							backFireToggle = false;
							backFireDelay = 0;

							double xTimes = random.Float();
							if (xTimes <= 0.33) {
								temp1toggle = true;
							}
							temp1 = false;
					}
				}

				if (temp1toggle) {
					temp1delay += gameTimer->getDeltaTime();
					if (temp1delay >= 0.2) {
						temp1 = true;
						temp1toggle = false;
						temp1delay = 0;
					}
				}

				backFireDelay += gameTimer->getDeltaTime();
				if (backFireDelay >= 4) {
					backFireToggle = true;
				}

				int xTimes = roundf(random.Float());


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

		// Incase the car has turned around and cant get up
		if (keyboard_state_array[SDL_SCANCODE_R] && resetCarToggle) {
			resetCarToggle = false;
			resetCarDelay = 0;

			vehicle->vehicle->getRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
			vehicle->setInitialPosition(latestReachedCheckpoint->getWorldTransform().getOrigin() + btVector3(0, 3, 0));
			vehicle->setInitialRotation(latestReachedCheckpoint->getWorldTransform().getRotation());
		}
		resetCarDelay += gameTimer->getDeltaTime();
		if (resetCarDelay >= 4) {
			resetCarToggle = true;
		}

		if (keyboard_state_array[SDL_SCANCODE_T]) { // for testing purpose

			cout << vehicle->getTransform().getOrigin().x() << endl;
			cout << vehicle->getTransform().getOrigin().y() << endl;
			cout << vehicle->getTransform().getOrigin().z() << endl;

		}

		// Complete reset the vehicle to a certain position. Could be used for checkpoints
		if (keyboard_state_array[SDL_SCANCODE_0] && completeResetCarToggle) {
			completeResetCarToggle = false;
			completeResetCarDelay = 0;
			vehicle->setInitialPosition(btVector3(10, 1, 10)); // certain position
			vehicle->setInitialRotation(btQuaternion(0, 0, 0, 1));
		}
		completeResetCarDelay += gameTimer->getDeltaTime();
		if (completeResetCarDelay >= 4) {
			completeResetCarToggle = true;
		}

		// Different perspectives
		if (keyboard_state_array[SDL_SCANCODE_1] || (buttons & SDL_BUTTON_LMASK) == SDL_BUTTON_LMASK) {
			perspective = 1;
			camOrientation = glm::vec3(0, 1, 0);
			cameraDistance = 11;
			cameraDistance2 = 3;
		}
		if ((keyboard_state_array[SDL_SCANCODE_C] || (buttons & SDL_BUTTON_RMASK) == SDL_BUTTON_RMASK) && perspective != 3) {
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
			camOffset = glm::vec3(cameraDistance * vehicle->getOrientation().x, cameraDistance * vehicle->getOrientation().y + cameraDistance2, cameraDistance * vehicle->getOrientation().z); //offset xx. Height xx
			// Interpolation on camdirection and position which creates a delay. More smooth camera movement. More immersive
			camDirection = camPosition + (vehicle->getPosition() - camPosition) * 0.5f;
			camPosition = camPosition + (camOffset + vehicle->getPosition() - camPosition) * 0.1f;
		}
		if (perspective == 1 && keyboard_state_array[SDL_SCANCODE_V]) {
			cameraDistance = 6;
			cameraDistance2 = 1.5;
		}
		// Perspective 2 => for reversing
		else if (perspective == 2) {
			camOffset = glm::vec3(15 * vehicle->getOrientation().x * -1, -15 * vehicle->getOrientation().y + 3, 15 * vehicle->getOrientation().z * -1); //offset 20. Height 3
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
			soundString[1] = '1';
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

		if (keyboard_state_array[SDL_SCANCODE_8]) {
			checkpointsReached = 8;
		}

		if (keyboard_state_array[SDL_SCANCODE_9]) {
			checkpointsReached = 9;
		}

		// Update "self" sound source
		sound->updateSoundSource(0, vehicle->getPosition(), vehicle->getVelocity(), vehicle->getSpeed(), soundString);
		sound->setListenerParameters(camPosition, camDirection, vehicle->getVelocity(), vehicle->getSpeed());
		sound->playSourcelessSounds(camPosition);
		
		// Reset sounds
		soundString = "00000";
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