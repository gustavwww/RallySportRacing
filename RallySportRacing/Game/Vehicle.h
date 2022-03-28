#include "GameObject.h"
#include <btBulletDynamicsCommon.h>

class Vehicle : public GameObject{

public:
	Vehicle(Rendering::Model* model, btDiscreteDynamicsWorld* dynamicsWorld);
	~Vehicle();
	btRaycastVehicle* vehicle;
	virtual void updateTransform();
	void drive(int direction);
	void steerRight(double deltaTime);
	void steerLeft(double deltaTime);
	void steerNeutral();
	void Vehicle::notGasing();
	
protected:
	float steering;
	float steeringClamp;
	float steeringIncrement;
	float engineForce;
	btVehicleRaycaster* raycaster;
};
