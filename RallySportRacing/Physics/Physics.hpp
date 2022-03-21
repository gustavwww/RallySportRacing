#ifndef physics_hpp
#define physics_hpp

//#include <headers/btBulletDynamicsCommon.h>
#include "../../External/bullet/headers/btBulletDynamicsCommon.h"

class Physics {

public:
	Physics();
	btDiscreteDynamicsWorld* dynamicsWorld;

private:
	btBroadphaseInterface* broadphase;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;
};

#endif