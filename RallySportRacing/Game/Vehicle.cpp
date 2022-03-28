#include "Vehicle.h"
#include <iostream>


Vehicle::Vehicle(Rendering::Model* model, btDiscreteDynamicsWorld* dynamicsWorld) : GameObject(model, dynamicsWorld)
{
	steeringClamp = 0.2;
	steeringIncrement = 0.5;
	engineForce = 5;

	btScalar chassisMass(1.0);
	btVector3 chassisInertia(0.0f, 0.0f, 0.0f);
	btVector3 shape = btVector3(0.9, 0.5, 2.3);
	collisionShape = new btBoxShape(shape); // meshCollider = new Mesh(collider); =>  collisionShape = new btConvexTriangleMeshShape(meshCollider->meshInterface);

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

	tuning.m_suspensionStiffness = 12.88;
	tuning.m_suspensionDamping = 3.0;
	tuning.m_suspensionCompression = 10.1;
	tuning.m_maxSuspensionForce = 11600.0;
	tuning.m_maxSuspensionTravelCm = 30.0;
	tuning.m_frictionSlip = 100.5;

	vehicle = new btRaycastVehicle(tuning, rigidBody, raycaster);
	vehicle->setCoordinateSystem(0, 1, 2);

	/*btVector3 wheelDirection(0.0f, -1.0f, 0.0f);
	btVector3 wheelAxis(-1.0f, 0.0f, 0.0f);
	btScalar suspensionRestLength(0.1);
	btScalar wheelRadius(0.3f);*/

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

	/*vehicle->addWheel(btVector3(-0.7f, 0.0f, 1.7f), wheelDirection, wheelAxis, suspensionRestLength, wheelRadius, tuning, true);
	vehicle->addWheel(btVector3(0.7f, 0.0f, 1.7f), wheelDirection, wheelAxis, suspensionRestLength, wheelRadius, tuning, true);
	vehicle->addWheel(btVector3(-0.7f, 0.0f, -1.45f), wheelDirection, wheelAxis, suspensionRestLength, wheelRadius, tuning, false);
	vehicle->addWheel(btVector3(0.7f, 0.0f, -1.45f), wheelDirection, wheelAxis, suspensionRestLength, wheelRadius, tuning, false);
	*/

	for (int i = 0; i < vehicle->getNumWheels(); i++)
	{
		btWheelInfo& wheel = vehicle->getWheelInfo(i);
		wheel.m_suspensionStiffness = 50;
		wheel.m_wheelsDampingCompression = btScalar(0.3) * 2 * btSqrt(wheel.m_suspensionStiffness);//btScalar(0.8);
		wheel.m_wheelsDampingRelaxation = btScalar(0.5) * 2 * btSqrt(wheel.m_suspensionStiffness);//1;
		//Larger friction slips will result in better handling
		wheel.m_frictionSlip = btScalar(3);
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
	//cout << "Position " << vehicle->getChassisWorldTransform().getOrigin().z();
}

void Vehicle::notGasing()
{
	vehicle->applyEngineForce(0, 2);
	vehicle->applyEngineForce(0, 3);

	//Default braking force, always added otherwise there is no friction on the wheels
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
		vehicle->updateWheelTransform(i, true);
		//vehicle->getWheelInfo(1).m_raycastInfo.m_groundObject
		//wheels[i]->updateTransform(vehicle->getWheelInfo(i).m_worldTransform); Denna raden gör så att hjulen som objekt faktiskt uppdaterar sin position och roteras beroende på styrningen osv
	}
}


