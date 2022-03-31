#include "Vehicle.h"
#include <iostream>

namespace Game {

	Vehicle::Vehicle(Rendering::Model* model, btDiscreteDynamicsWorld* dynamicsWorld, GameObject* wheel1, GameObject* wheel2, GameObject* wheel3, GameObject* wheel4) : GameObject(model, 0, dynamicsWorld)
	{
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

		btVector3 wheelDirectionCS0(0, -1, 0);

		//The axis which the wheel rotates arround
		btVector3 wheelAxleCS(-1.0f, 0.0f, 0.0f);

		btScalar suspensionRestLength(0.7);

		btScalar wheelWidth(0.4);

		btScalar wheelRadius(0.5);

		//The height where the wheels are connected to the chassis
		btScalar connectionHeight(shape.y() - wheelRadius);

		//All the wheel configuration assumes the vehicle is centered at the origin and a right handed coordinate system is used
		btVector3 wheelConnectionPoint(shape.x(), connectionHeight, shape.z() - wheelWidth); // these will be needed to be adjusted depending on the model

		//Adds the front wheels
		vehicle->addWheel(wheelConnectionPoint, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, true);

		vehicle->addWheel(wheelConnectionPoint * btVector3(-1, 1, 1), wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, true);

		//Adds the rear wheels
		vehicle->addWheel(wheelConnectionPoint * btVector3(1, 1, -1), wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, false);

		vehicle->addWheel(wheelConnectionPoint * btVector3(-1, 1, -1), wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, false);

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

		//Default braking force, always added otherwise there is no friction on the wheels, (This is axis friction)
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

		//m_groundObject = vehicle->getWheelInfo(1).m_raycastInfo.m_groundObject;
		//cout << "object: " << m_groundObject <<  endl;
		//btRigidBody* test = btRigidBody::upcast(raycaster.m_groundObject);
		// 
		//btCollisionObject* groundObject =
			//static_cast<btCollisionObject*>(vehicle->getWheelInfo(1).m_raycastInfo.m_groundObject);

		//class btRigidBody* groundObject =
			//(class btRigidBody*)vehicle->getWheelInfo(1).m_raycastInfo.m_groundObject;

		//if (groundObject->getFriction() != 0) {
			//float test = *groundObject;
			//cout << "friction: " << test << endl;
		//}



		for (int i = 0; i < 4; i++) {
			vehicle->updateWheelTransform(i, true);
			wheels[i]->updateTransform(vehicle->getWheelInfo(i).m_worldTransform);
			//vehicle->getWheelInfo(1).m_raycastInfo.m_groundObject
			//wheels[i]->updateTransform(vehicle->getWheelInfo(i).m_worldTransform); Denna raden gör så att hjulen som objekt faktiskt uppdaterar sin position och roteras beroende på styrningen osv
		}
	}


}
