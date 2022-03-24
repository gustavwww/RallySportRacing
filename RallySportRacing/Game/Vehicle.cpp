#include "Vehicle.h"
#include <iostream>

Vehicle::Vehicle(Rendering::Model* model, btDiscreteDynamicsWorld* dynamicsWorld) : GameObject(model, dynamicsWorld)
{
	steeringClamp = 0.2;
	steeringIncrement = 0.5;
	engineForce = 1.5;

	btScalar chassisMass(1.0);
	btVector3 chassisInertia(0.0f, 0.0f, 0.0f);
	collisionShape = new btBoxShape(btVector3(0.7, 0.25, 1.25)); // meshCollider = new Mesh(collider); =>  collisionShape = new btConvexTriangleMeshShape(meshCollider->meshInterface);

	btQuaternion initalRotation = btQuaternion(0, 0, 0, 1);
	initalRotation.setRotation(btVector3(0, 1, 0), btScalar(0));
	motionState = new btDefaultMotionState(btTransform(initalRotation, btVector3(0.f, 30.f, 0.f)));
	collisionShape->calculateLocalInertia(chassisMass, chassisInertia);
	btRigidBody::btRigidBodyConstructionInfo chassisRigidBodyCI(chassisMass, motionState, collisionShape, chassisInertia);
	rigidBody = new btRigidBody(chassisRigidBodyCI);
	rigidBody->setActivationState(DISABLE_DEACTIVATION);
	dynamicsWorld->addRigidBody(rigidBody);

	raycaster = new btDefaultVehicleRaycaster(dynamicsWorld);
	btRaycastVehicle::btVehicleTuning tuning;

	tuning.m_suspensionStiffness = 12.88;
	tuning.m_suspensionDamping = 3.0;
	tuning.m_suspensionCompression = 10.1;
	tuning.m_maxSuspensionForce = 11600.0;
	tuning.m_maxSuspensionTravelCm = 30.0;
	tuning.m_frictionSlip = 100.5;

	vehicle = new btRaycastVehicle(tuning, rigidBody, raycaster);
	vehicle->setCoordinateSystem(0, 1, 2);

	btVector3 wheelDirection(0.0f, -1.0f, 0.0f);
	btVector3 wheelAxis(-1.0f, 0.0f, 0.0f);
	btScalar suspensionRestLength(0.1);
	btScalar wheelRadius(0.4f);

	vehicle->addWheel(btVector3(-0.7f, 0.0f, 0.8f), wheelDirection, wheelAxis, suspensionRestLength, wheelRadius, tuning, true);
	vehicle->addWheel(btVector3(0.7f, 0.0f, 0.8f), wheelDirection, wheelAxis, suspensionRestLength, wheelRadius, tuning, true);
	vehicle->addWheel(btVector3(-0.7f, 0.0f, -1.45f), wheelDirection, wheelAxis, suspensionRestLength, wheelRadius, tuning, false);
	vehicle->addWheel(btVector3(0.7f, 0.0f, -1.45f), wheelDirection, wheelAxis, suspensionRestLength, wheelRadius, tuning, false);

	dynamicsWorld->addVehicle(vehicle);

}

Vehicle::~Vehicle()
{
	delete raycaster;
	delete vehicle;
}

void Vehicle::drive(int direction)
{
	vehicle->applyEngineForce(direction * engineForce, 0);
	vehicle->applyEngineForce(direction * engineForce, 1);
	//cout << "Position " << vehicle->getChassisWorldTransform().getOrigin().z();
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

void Vehicle::updateTransform()
{
	GameObject::updateTransform();

	for (int i = 0; i < 4; i++) {
		vehicle->updateWheelTransform(i, true);
		//wheels[i]->updateTransform(vehicle->getWheelInfo(i).m_worldTransform); Denna raden gör så att hjulen som objekt faktiskt uppdaterar sin position och roteras beroende på styrningen osv
	}
}

