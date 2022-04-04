#include "Vehicle.h"
#include <iostream>
#include <Game/GameManager.h>

namespace Game {


	Vehicle::Vehicle(Rendering::Model* model, btDiscreteDynamicsWorld* dynamicsWorld) : GameObject(model, dynamicsWorld)
	{
		Rendering::Model* wheel1Model = Rendering::Model::loadModel("../Models/SimpleCarAppliedTransforms.gltf");
		getHandler()->addModel(wheel1Model);
		Rendering::Model* wheel2Model = Rendering::Model::loadModel("../Models/SimpleCarAppliedTransforms.gltf");
		getHandler()->addModel(wheel2Model);
		Rendering::Model* wheel3Model = Rendering::Model::loadModel("../Models/SimpleCarAppliedTransforms.gltf");
		getHandler()->addModel(wheel3Model);
		Rendering::Model* wheel4Model = Rendering::Model::loadModel("../Models/SimpleCarAppliedTransforms.gltf");
		getHandler()->addModel(wheel4Model);

		isWheel = 1;

		//test wheels but have no model, using wallmodel temporary
		wheel1 = new GameObject(wheel1Model, isWheel, dynamicsWorld);
		wheel2 = new GameObject(wheel2Model, isWheel, dynamicsWorld);
		wheel3 = new GameObject(wheel3Model, isWheel, dynamicsWorld);
		wheel4 = new GameObject(wheel4Model, isWheel, dynamicsWorld);

		wheels.push_back(wheel1);
		wheels.push_back(wheel2);
		wheels.push_back(wheel3);
		wheels.push_back(wheel4);


		steeringClamp = 0.2;
		steeringIncrement = 0.5;
		engineForce = 5;

		btScalar chassisMass(1.0);
		btVector3 chassisInertia(0.0f, 0.0f, 0.0f);
		btVector3 shape = btVector3(0.9, 0.5, 2.3); // could use automatically generated cshape from model but requires a little bit of fine tunements
		collisionShape = new btBoxShape(shape); 

		btQuaternion initalRotation = btQuaternion(0, 0, 0, 1);
		initalRotation.setRotation(btVector3(0, 1, 0), btScalar(0));
		motionState = new btDefaultMotionState(btTransform(initalRotation, btVector3(0.f, 10.f, 0.f)));
		collisionShape->calculateLocalInertia(chassisMass, chassisInertia);

		// moves the centrum of the collisionshape
		compoundShape = new btCompoundShape();
		btTransform position;
		position.setIdentity();
		position.setOrigin(btVector3(0, 1, 0.25));  // offset
		compoundShape->addChildShape(position, collisionShape);
		shape = shape + btVector3(0, 1, -0.25); // adjust shape with offset 

		btRigidBody::btRigidBodyConstructionInfo chassisRigidBodyCI(chassisMass, motionState, compoundShape, chassisInertia);
		rigidBody = new btRigidBody(chassisRigidBodyCI);
		rigidBody->setActivationState(DISABLE_DEACTIVATION);
		dynamicsWorld->addRigidBody(rigidBody);

		raycaster = new btDefaultVehicleRaycaster(dynamicsWorld);
		btRaycastVehicle::btVehicleTuning tuning;

		tuning.m_suspensionStiffness = 20.f;
		tuning.m_suspensionDamping = 2.3f;
		tuning.m_suspensionCompression = 4.4f;
		tuning.m_maxSuspensionForce = 11600.0;
		tuning.m_maxSuspensionTravelCm = 30.0;
		tuning.m_frictionSlip = 100.5;


		vehicle = new btRaycastVehicle(tuning, rigidBody, raycaster);
		vehicle->setCoordinateSystem(0, 1, 2);

		btVector3 wheelDirection(0, -1, 0);

		//The axis which the wheel rotates arround
		btVector3 wheelAxle(-1.0f, 0.0f, 0.0f);

		btScalar suspensionRestLength(0.7);

		btScalar wheelWidth(0.4);

		btScalar wheelRadius(0.5);

		//The height where the wheels are connected to the chassis
		btScalar connectionHeight(shape.y() - wheelRadius);

		//All the wheel configuration assumes the vehicle is centered at the origin and a right handed coordinate system is used
		btVector3 wheelConnectionPoint(shape.x(), connectionHeight, shape.z() - wheelWidth); // these will be needed to be adjusted depending on the model

		//Adds the front wheels
		vehicle->addWheel(wheelConnectionPoint, wheelDirection, wheelAxle, suspensionRestLength, wheelRadius, tuning, true);

		vehicle->addWheel(wheelConnectionPoint * btVector3(-1, 1, 1), wheelDirection, wheelAxle, suspensionRestLength, wheelRadius, tuning, true);

		//Adds the rear wheels
		vehicle->addWheel(wheelConnectionPoint * btVector3(1, 1, -1), wheelDirection, wheelAxle, suspensionRestLength, wheelRadius, tuning, false);

		vehicle->addWheel(wheelConnectionPoint * btVector3(-1, 1, -1), wheelDirection, wheelAxle, suspensionRestLength, wheelRadius, tuning, false);

		for (int i = 0; i < vehicle->getNumWheels(); i++)
		{
			btWheelInfo& wheel = vehicle->getWheelInfo(i);
			wheel.m_suspensionStiffness = 50;
			wheel.m_wheelsDampingCompression = btScalar(0.3) * 2 * btSqrt(wheel.m_suspensionStiffness);
			wheel.m_wheelsDampingRelaxation = btScalar(0.5) * 2 * btSqrt(wheel.m_suspensionStiffness);
			wheel.m_frictionSlip = btScalar(4);
			wheel.m_rollInfluence = 1;
			
		}

		dynamicsWorld->addVehicle(vehicle);

		
	}

	Vehicle::~Vehicle()
	{
		delete raycaster;
		delete vehicle;
	}

	void Vehicle::drive(int direction)
	{
		vehicle->applyEngineForce(direction * engineForce, 2);
		vehicle->applyEngineForce(direction * engineForce, 3);
	}

	void Vehicle::notGasing()
	{
		vehicle->applyEngineForce(0, 2);
		vehicle->applyEngineForce(0, 3);

		// Default braking force, always added otherwise there is no friction on the wheels, (This is axis friction)
		// I can change this depending on ground? One way to simulate friction. Same with frictionslip
		// Real rolling friction is not possible using a raycastvehicle because a ray is infinitely thin and thus cannot cause friction. Other wise you would have to simulate a rolling cylinder
		vehicle->setBrake(1, 2);
		vehicle->setBrake(1, 3);
	}

	void Vehicle::steerRight(double deltaTime)
	{
		steering += steeringIncrement * deltaTime;
		steering = std::fmin(steering, steeringClamp);
		vehicle->setSteeringValue(steering, 0);
		vehicle->setSteeringValue(steering, 1);
	}

	void Vehicle::steerLeft(double deltaTime)
	{
		steering -= steeringIncrement * deltaTime;
		steering = std::fmax(steering, -steeringClamp);
		vehicle->setSteeringValue(steering, 0);
		vehicle->setSteeringValue(steering, 1);
	}
	void Vehicle::steerNeutral()
	{
		steering = 0.0f;
		vehicle->setSteeringValue(steering, 0);
		vehicle->setSteeringValue(steering, 1);
	}

	void Vehicle::handBrake()
	{
		vehicle->applyEngineForce(0, 2);
		vehicle->applyEngineForce(0, 3);

		vehicle->setBrake(500, 2);
		vehicle->setBrake(500, 3);
	}

	void Vehicle::updateTransform()
	{
		GameObject::updateTransform();

		for (int i = 0; i < 4; i++) {

			if (vehicle->getWheelInfo(i).m_raycastInfo.m_isInContact) {

				btTransform location = vehicle->getWheelInfo(i).m_worldTransform;
				btVector3 start = location.getOrigin();
				btVector3 wheelRadiusOffset = btVector3(0, vehicle->getWheelInfo(i).m_wheelsRadius, 0);
				btVector3 end = start - wheelRadiusOffset;


				btCollisionWorld::ClosestRayResultCallback RayCallback(start, end);

				dynamicsWorld->rayTest(
					start,
					end,
					RayCallback
				);

				if (RayCallback.hasHit()) {
					float friction = (float)RayCallback.m_collisionObject->getFriction();
					vehicle->setBrake(friction, 2); // does not work when setting wheel 0 and 1
					vehicle->setBrake(friction, 3); // does not work when setting wheel 0 and 1
					vehicle->getWheelInfo(i).m_frictionSlip = friction * static_cast<btScalar>(10);

					cout << "friction " << friction << endl;
					cout << "frictionSlip " << friction * static_cast<btScalar>(10) << endl;
				}

			}

			vehicle->updateWheelTransform(i, true);
			wheels[i]->updateTransform(vehicle->getWheelInfo(i).m_worldTransform);
		}
	}


}