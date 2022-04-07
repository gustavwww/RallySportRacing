#include "GameObject.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <btBulletDynamicsCommon.h>
#include <iostream>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include <SDL.h>

#define PI 3.1415926538f


namespace Game{

	GameObject::GameObject(Rendering::Model* model, string objectType, btDiscreteDynamicsWorld* dynamicsWorld) : dynamicsWorld(dynamicsWorld) {
		this->model = model;
		position = glm::vec3(0.0f);
		orientation = glm::vec3(0.0f);
		quaternion= glm::quat(0, 0, 0, 0);
		this->objectType = objectType;
		setupRigidbody();
	}

	GameObject::GameObject(Rendering::Model* model, btDiscreteDynamicsWorld* dynamicsWorld) : dynamicsWorld(dynamicsWorld) {
		this->model = model;
		position = glm::vec3(0.0f);
		orientation = glm::vec3(0.0f);
		quaternion = glm::quat(0, 0, 0, 0);
		setupRigidbody();
	}

	GameObject::GameObject(Rendering::Model* model, string objectType, float friction, btDiscreteDynamicsWorld* dynamicsWorld) : dynamicsWorld(dynamicsWorld) {
		this->model = model;
		position = glm::vec3(0.0f);
		orientation = glm::vec3(0.0f);
		quaternion = glm::quat(0, 0, 0, 0);
		this->objectType = objectType;
		this->friction = friction;
		setupRigidbody();
	}

	Game::GameObject::GameObject(Rendering::Model* model)
	{
		this->model = model;
		position = glm::vec3(0.0f);
		orientation = glm::vec3(0.0f);
		quaternion = glm::quat(0,0,0,0);
	}

	void GameObject::updateMatrices() {
		position = bulletToGlm(transform.getOrigin());
		model->setTranslationMatrix(glm::translate(glm::mat4(1.0f), bulletToGlm(transform.getOrigin())));

		quaternion = bulletToGlm(transform.getRotation());

		orientation = glm::rotate(quaternion, glm::vec3(0, 0, -1));
	
		(*model).setRotationMatrix(glm::toMat4(quaternion));


	}

	void Game::GameObject::setPosition(glm::vec3 position)
	{
		this->position = position;
		model->setTranslationMatrix(glm::translate(glm::mat4(1.0f), position));
	}

	void Game::GameObject::setQuaternion(glm::quat quaternion)
	{
		(*model).setRotationMatrix(glm::rotate(glm::toMat4(quaternion), PI, glm::vec3(0, 0, 1)));
	}

	glm::quat Game::GameObject::getQuaternion()
	{
		return quaternion;
	}

	glm::vec3 GameObject::getPosition() {
		return position;
	}

	glm::vec3 GameObject::getOrientation() {
		return orientation;
	}

	void GameObject::setupRigidbody() {
	
		compoundShape = new btCompoundShape();

		// this ifstatement does not do anything. It gets overwritten by the information from the vechicle class anyways.
		// However the concept is applicable on heightmaps and that is why I have not removed it yet
		if (objectType == "wheel") { // special case for wheels, maybe we want to add another one for heightmaps?
			collisionShape = new btCylinderShape(btVector3(0.1, 0.1, 0.1));
			btTransform position;
			position.setIdentity();
			compoundShape->addChildShape(position, collisionShape);
		}
		else if (objectType == "terrain") { // for terrainShapes based on heightmaps

			
			//SDL_Surface* image = IMG_Load("test/test.png");

			// btHeightfieldTerrainShape (int heightStickWidth, int heightStickLength, const void *heightfieldData, btScalar heightScale, btScalar minHeight, btScalar maxHeight, int upAxis, PHY_ScalarType heightDataType, bool flipQuadEdges)

			/*float numHeightfieldRows = 256;
			float numHeightfieldColumns = 256;
			float heightfieldData[] = {0};
			for (int j = 0; j <= int(numHeightfieldColumns / 2); j++) {
				for (int i = 0; i <= int(numHeightfieldRows / 2); i++) {
					height = random.uniform(0, heightPerturbationRange)
						heightfieldData[2 * i + 2 * j * numHeightfieldRows] = height
						heightfieldData[2 * i + 1 + 2 * j * numHeightfieldRows] = height
						heightfieldData[2 * i + (2 * j + 1) * numHeightfieldRows] = height
						heightfieldData[2 * i + 1 + (2 * j + 1) * numHeightfieldRows] = height
				}
			}*/

			//float test[] = {1, 1, 1, 0, 0, 0, 4};

			//collisionShape = new btHeightfieldTerrainShape(1, 1, 0, 1, 0, 1, 1, PHY_FLOAT, false); // test values

			collisionShape = new btBvhTriangleMeshShape(model->getMeshInterface(), false, true);

			btTransform position;
			position.setIdentity();
			compoundShape->addChildShape(position, collisionShape);

		}
		else { // creates collisionshape based on model and its offset
			collisionShape = new btBoxShape(model->generateCollisionShape());
			btTransform position;
			position.setIdentity();
			position.setOrigin(btVector3(model->generateCollisionShapeOffset()));  // offset
			compoundShape->addChildShape(position, collisionShape);

		}

		motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
		btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, motionState, compoundShape, btVector3(0, 0, 0));
		rigidBody = new btRigidBody(groundRigidBodyCI);

		rigidBody->setFriction(friction);

		rigidBody->setContactProcessingThreshold(0.f);

		btTransform initialTransform;
		initialTransform.setOrigin(btVector3(0, -5, 0));
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
		btTransform btTransform;
		rigidBody->getMotionState()->getWorldTransform(btTransform);
		transform = btTransform;
		updateMatrices();
	}

	void GameObject::updateTransform(const btTransform& btTransform)
	{
		transform = btTransform;
		updateMatrices();
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
}