#ifndef GAMEOBJECT
#define GAMEOBJECT

#include "Rendering/Model.h"
#include <glm/glm.hpp>
#include <btBulletDynamicsCommon.h>
#include <glm/gtx/quaternion.hpp>

namespace Game {
	class GameObject {
	public:
		GameObject(Rendering::Model* model, btDiscreteDynamicsWorld* dynamicsWorld, string name); // for objects not interatcting with physics (no friction)
		GameObject(Rendering::Model* model, bool isTerrain, float friction, btDiscreteDynamicsWorld* dynamicsWorld); // normal
		GameObject(Rendering::Model* model); // for multiplayer

		~GameObject();


		void updateMatrices();

		btTransform getTransform();
		void setInitialPosition(const btVector3& v);
		void setInitialRotation(const btQuaternion& q);
		void setupRigidbody();
		virtual void updateTransform();
		virtual void updateTransform(const btTransform& btTransform);

		void setPosition(glm::vec3 position);
		void setQuaternion(glm::quat q);
		glm::quat getQuaternion();
		glm::vec3 getPosition();
		glm::vec3 getOrientation();
		btVector3 glmToBullet(const glm::vec3& vec);
		btQuaternion glmToBullet(const glm::quat& q);
		glm::vec3 bulletToGlm(const btVector3& v);
		glm::quat bulletToGlm(const btQuaternion& q);

		Rendering::Model* getModel();
		string getName();

	protected:
		string name;
		Rendering::Model* model;
		glm::vec3 position;
		glm::vec3 orientation;
		glm::quat quaternion;
		bool isTerrain = 0;
		float friction = 0;

		btTransform transform;
		btDiscreteDynamicsWorld* dynamicsWorld;
		btRigidBody* rigidBody;
		btDefaultMotionState* motionState;
		btCollisionShape* collisionShape;
		btCompoundShape* compoundShape;

	};
}
#endif
