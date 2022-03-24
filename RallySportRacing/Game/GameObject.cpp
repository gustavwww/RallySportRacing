#include "GameObject.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <btBulletDynamicsCommon.h>
#include <iostream>


GameObject::GameObject(Rendering::Model* model, btDiscreteDynamicsWorld* dynamicsWorld) : dynamicsWorld(dynamicsWorld) {
	this->model = model;
	position = glm::vec3(0.0f);
	orientation = glm::vec3(0.0f);
	setupRigidbody();
}

void GameObject::updateMatrices() {
	position = bulletToGlm(getTransform().getOrigin());
	model->setTranslationMatrix(glm::translate(glm::mat4(1.0f), bulletToGlm(rigidBody->getWorldTransform().getOrigin())));

	glm::quat q = bulletToGlm(rigidBody->getWorldTransform().getRotation());
	glm::vec3 euler = glm::eulerAngles(q); // gives xyz
	orientation = glm::vec3 (euler.y, euler.x, euler.z);
	(*model).setRotationMatrix(glm::eulerAngleYXZ(euler.y, euler.x, euler.z));

	//model->setTranslationMatrix(glm::translate(glm::mat4(1.0f), position));
	//std::cout << "RigidX: " << rigidBody->getWorldTransform().getOrigin().getX();
	//std::cout << "Positionx: " << position.x << endl;
}

void GameObject::rotate(glm::vec3 angleVector) {
	/*(*model).setRotationMatrix(glm::toMat4(glm::quat(transform.getRotation().getW(),
		transform.getRotation().getX(),
		transform.getRotation().getY(),
		transform.getRotation().getZ())));*/
		//(*model).setRotationMatrix(glm::toMat4(bulletToGlm(transform.getRotation())));
}

glm::vec3 GameObject::getPosition() {
	return position;
}

glm::vec3 GameObject::getOrientation() {
	return orientation;
}

void GameObject::setupRigidbody() {
	//collisionShape = new btBvhTriangleMeshShape(model->meshInterface, false, true);
	collisionShape = new btBoxShape(btVector3(100, 2, 100));

	motionState = new btDefaultMotionState(btTransform(btQuaternion(1, 0, 0, 1), btVector3(0, 0, 0)));
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, motionState, collisionShape, btVector3(0, 0, 0));
	rigidBody = new btRigidBody(groundRigidBodyCI);
	rigidBody->setContactProcessingThreshold(0.f);


	btTransform initialTransform;
	initialTransform.setOrigin(glmToBullet(position));
	initialTransform.setRotation(btQuaternion(0, 0, 0, 1));
	rigidBody->setWorldTransform(initialTransform);
	motionState->setWorldTransform(initialTransform);


	dynamicsWorld->addRigidBody(rigidBody);
}

GameObject::~GameObject() {
	delete model;
	delete rigidBody;
	delete motionState;
	delete collisionShape;
}

btTransform GameObject::getTransform() {
	return transform;
}
void GameObject::setInitialPosition(const btVector3& v)
{
	btTransform initialTransform;
	initialTransform.setOrigin(v);
	initialTransform.setRotation(rigidBody->getWorldTransform().getRotation());
	rigidBody->setWorldTransform(initialTransform);
	motionState->setWorldTransform(initialTransform);
}
void GameObject::setInitialRotation(const btQuaternion& q)
{
	btTransform initialTransform;
	initialTransform.setOrigin(rigidBody->getWorldTransform().getOrigin());
	initialTransform.setRotation(q);
	rigidBody->setWorldTransform(initialTransform);
	motionState->setWorldTransform(initialTransform);
}


void GameObject::updateTransform()
{
	//btTransform initialTransform;

	//initialTransform.setOrigin(glmToBullet(position));
	//initialTransform.setRotation(glmToBullet(glm::quat(orientation)));

	//rigidBody->setWorldTransform(initialTransform);
	//motionState->setWorldTransform(initialTransform);
	//rigidBody->applyForce(btVector3(1, 1, 1), btVector3(1, 1, 1));
	/*
	btTransform btTransform;
	rigidBody->getMotionState()->getWorldTransform(btTransform);
	transform = btTransform;
	*/
	//rigidBody->applyCentralForce(btVector3(100, 0, 0));

	btTransform btTransform;
	rigidBody->getMotionState()->getWorldTransform(btTransform);
	transform = btTransform;
	updateMatrices();
	
}

void GameObject::updateTransform(const btTransform& btTransform)
{
	//std::cout << "RigidX: " << rigidBody->getWorldTransform().getOrigin().getX();
	transform = btTransform;
}

// Different types of conversions between bullet and glm
btVector3 GameObject::glmToBullet(const glm::vec3& vec)
{
	return { vec.x, vec.y, vec.z };
}
btQuaternion GameObject::glmToBullet(const glm::quat& q)
{
	return btQuaternion(q.x, q.y, q.z, q.w);
}
glm::vec3 GameObject::bulletToGlm(const btVector3& v)
{
	return glm::vec3(v.getX(), v.getY(), v.getZ());
}
glm::quat GameObject::bulletToGlm(const btQuaternion& q)
{
	return glm::quat(q.getW(), q.getX(), q.getY(), q.getZ());
}




//nedan är konverteringar mellan glm vectorer och bulletsvectorer osv
/*glm::vec3 bulletToGlm(const btVector3& v) { return glm::vec3(v.getX(), v.getY(), v.getZ()); }

btVector3 glmToBullet(const glm::vec3& v) { return btVector3(v.x, v.y, v.z); }

glm::quat bulletToGlm(const btQuaternion& q) { return glm::quat(q.getW(), q.getX(), q.getY(), q.getZ()); }

btQuaternion glmToBullet(const glm::quat& q) { return btQuaternion(q.x, q.y, q.z, q.w); }

btMatrix3x3 glmToBullet(const glm::mat3& m) { return btMatrix3x3(m[0][0], m[1][0], m[2][0], m[0][1], m[1][1], m[2][1], m[0][2], m[1][2], m[2][2]); }

// btTransform does not contain a full 4x4 matrix, so this transform is lossy.
// Affine transformations are OK but perspective transformations are not.
btTransform glmToBullet(const glm::mat4& m)
{
	glm::mat3 m3(m);
	return btTransform(glmToBullet(m3), glmToBullet(glm::vec3(m[3][0], m[3][1], m[3][2])));
}

glm::mat4 bulletToGlm(const btTransform& t)
{
	glm::mat4 m(glm::mat4::_null);
	const btMatrix3x3& basis = t.getBasis();
	// rotation
	for (int r = 0; r < 3; r++)
	{
		for (int c = 0; c < 3; c++)
		{
			m[c][r] = basis[r][c];
		}
	}
	// traslation
	btVector3 origin = t.getOrigin();
	m[3][0] = origin.getX();
	m[3][1] = origin.getY();
	m[3][2] = origin.getZ();
	// unit scale
	m[0][3] = 0.0f;
	m[1][3] = 0.0f;
	m[2][3] = 0.0f;
	m[3][3] = 1.0f;
	return m;
}*/
