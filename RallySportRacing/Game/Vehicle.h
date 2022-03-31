#include "GameObject.h"
#include <btBulletDynamicsCommon.h>

class Vehicle : public GameObject {

public:
	Vehicle(Rendering::Model* model, btDiscreteDynamicsWorld* dynamicsWorld, GameObject* wheel1, GameObject* wheel2, GameObject* wheel3, GameObject* wheel4);
	~Vehicle();
	btRaycastVehicle* vehicle;
	virtual void updateTransform();
	void drive(int direction);
	void steerRight(double deltaTime);
	void steerLeft(double deltaTime);
	void steerNeutral();
	void Vehicle::notGasing();
	void Vehicle::handBrake();

	void* m_groundObject; // the object the vehicle is in contact with
	
protected:
	float steering;
	float steeringClamp;
	float steeringIncrement;
	float engineForce;
	btVehicleRaycaster* raycaster;
	vector<GameObject*> wheels;
};
