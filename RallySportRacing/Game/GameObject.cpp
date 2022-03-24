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
		orientation += angleVector;
		(*model).setRotationMatrix(glm::eulerAngleYXZ(orientation.x, orientation.y, orientation.z));
	}

	void GameObject::setPosition(glm::vec3 position) {
		this->position = position;
		model->setTranslationMatrix(glm::translate(glm::mat4(1.0f), position));
	}

	void GameObject::setOrientation(glm::vec3 orientation) {
		this->orientation = orientation;
		model->setRotationMatrix(glm::eulerAngleXYZ(orientation.x, orientation.y, orientation.z));
	}

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