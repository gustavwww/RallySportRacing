#include "GameObject.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "btBulletDynamicsCommon.h"

namespace Game {

	GameObject::GameObject(Rendering::Model* model, btDiscreteDynamicsWorld* dynamicsWorld) : dynamicsWorld(dynamicsWorld) {
		this->model = model;
		position = glm::vec3(0.0f);
		orientation = glm::vec3(0.0f);
		setupRigidbody();
	}

	void GameObject::translate(glm::vec3 speedVector) {
		position += speedVector;
		model->setTranslationMatrix(glm::translate(glm::mat4(1.0f), position));
	}

	void GameObject::rotate(glm::vec3 angleVector) {
		orientation += angleVector;
		(*model).setRotationMatrix(glm::eulerAngleYXZ(orientation.x, orientation.y, orientation.z));
	}

	glm::vec3 GameObject::getPosition() {
		return position;
	}

	glm::vec3 GameObject::getOrientation() {
		return orientation;
	}

	void GameObject::setupRigidbody() {
		collisionShape = new btBvhTriangleMeshShape(model->meshInterface, false, true);
		motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
		btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, motionState, collisionShape, btVector3(0, 0, 0));
		rigidBody = new btRigidBody(groundRigidBodyCI);
		rigidBody->setContactProcessingThreshold(0.f);

		dynamicsWorld->addRigidBody(rigidBody);
	}

	GameObject::~GameObject() {
		delete model;
		delete rigidBody;
		delete motionState;
		delete collisionShape;
	}

	void GameObject::updateTransform()
	{
	}

	void GameObject::updateTransform( const btTransform& btTransform )
	{
	}

}