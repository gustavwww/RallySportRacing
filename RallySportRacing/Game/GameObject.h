#ifndef GAMEOBJECT
#define GAMEOBJECT

#include "Rendering/Model.h"
#include <glm/glm.hpp>
#include "btBulletDynamicsCommon.h"

namespace Game {

	class GameObject {
	public:
		GameObject(Rendering::Model* model, btDiscreteDynamicsWorld* dynamicsWorld);
		~GameObject();

		void translate(glm::vec3 speedVector);
		void rotate(glm::vec3 angleVector);

		btTransform getTransform();
		void setupRigidbody();
		virtual void updateTransform();
		virtual void updateTransform(const btTransform& btTransform);

		glm::vec3 getPosition();
		glm::vec3 getOrientation();
	private:
		Rendering::Model* model;
		glm::vec3 position;
		glm::vec3 orientation;

		btTransform transform;
		btDiscreteDynamicsWorld* dynamicsWorld;
		btRigidBody* rigidBody;
		btDefaultMotionState* motionState;
		btCollisionShape* collisionShape;

	};

}




#endif
