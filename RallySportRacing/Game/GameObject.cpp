#include "GameObject.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace Game {

	GameObject::GameObject(Rendering::Model* model) {
		this->model = model;
		position = glm::vec3(0.0f);
		orientation = glm::vec3(0.0f);
	}


	void GameObject::translate(glm::vec3 speedVector) {
		position += speedVector;
		model->setTranslationMatrix(glm::translate(glm::mat4(1.0f), position));
	}

	void GameObject::rotate(glm::vec3 angleVector) {
		//position += speedVector;
		(*model).setRotationMatrix(glm::eulerAngleYXZ(angleVector.x, angleVector.y, angleVector.z));
	}

	/*void GameObject::changeView(glm::vec3 angleVector) {
		// Params: Cam pos in World Space, where to look at, head up (0,-1,0) = upside down.
		glm::vec3 camOffset;
		camOffset = glm::vec3(20 * angleVector.x, 5, 20 * angleVector.z); //offset 20
		(*model).setViewMatrix(glm::lookAt(position + camOffset, position , glm::vec3(0, 1, 0)));
	}*/


	glm::vec3 GameObject::getPosition() {
		return position;
	}

	glm::vec3 GameObject::getOrientation() {
		return orientation;
	}

	GameObject::~GameObject() {
		delete model;
	}

}