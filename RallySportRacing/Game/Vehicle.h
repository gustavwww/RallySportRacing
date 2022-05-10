#ifndef VEHICLE
#define VEHICLE

#include "GameObject.h"
#include <btBulletDynamicsCommon.h>

namespace Game{
	class Vehicle : public GameObject {

	public:
		Vehicle(Rendering::Model* model, btDiscreteDynamicsWorld* dynamicsWorld);
		~Vehicle();
		btRaycastVehicle* vehicle;
		virtual void updateTransform();
		void drive(int direction);
		void steerRight(double deltaTime);
		void steerLeft(double deltaTime);
		void steerNeutral();
		void setMaxSpeed(int speed);
		void Vehicle::notGasing();
		void Vehicle::handBrake();
		float Vehicle::getSpeed();
		glm::vec3  Game::Vehicle::getVelocity();

		void* m_groundObject; // the object the vehicle is in contact with
	
		GameObject* wheel1;
		GameObject* wheel2;
		GameObject* wheel3;
		GameObject* wheel4;

	protected:
		int maxSpeed;
		float steering;
		float steeringClamp;
		float steeringIncrement;
		float engineForce;
		btVehicleRaycaster* raycaster;
		vector<GameObject*> wheels;
	};
}

#endif