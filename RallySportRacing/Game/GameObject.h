#ifndef GAMEOBJECT
#define GAMEOBJECT

#include "Rendering/Model.h"
#include <glm/glm.hpp>
#include <btBulletDynamicsCommon.h>



class GameObject {
public:
	GameObject(Rendering::Model* model, bool isWheel, btDiscreteDynamicsWorld* dynamicsWorld);
	~GameObject();

	void updateMatrices();

	btTransform getTransform();
	void setInitialPosition(const btVector3& v);
	void setInitialRotation(const btQuaternion& q);
	void setupRigidbody();
	virtual void updateTransform();
	virtual void updateTransform(const btTransform& btTransform);

	glm::vec3 getPosition();
	glm::vec3 getOrientation();
	btVector3 glmToBullet(const glm::vec3& vec);
	btQuaternion glmToBullet(const glm::quat& q);
	glm::vec3 bulletToGlm(const btVector3& v);
	glm::quat bulletToGlm(const btQuaternion& q);

protected:
	Rendering::Model* model;
	glm::vec3 position;
	glm::vec3 orientation;
	bool isWheel;

	btTransform transform;
	btDiscreteDynamicsWorld* dynamicsWorld;
	btRigidBody* rigidBody;
	btDefaultMotionState* motionState;
	btCollisionShape* collisionShape;
	btCompoundShape* compoundShape;

};






#endif
