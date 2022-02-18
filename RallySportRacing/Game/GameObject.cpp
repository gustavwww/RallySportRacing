#include "GameObject.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Game {

	GameObject::GameObject(Rendering::Model* model) {
		this->model = model;
		position = glm::vec3(0.0f);
	}


	void GameObject::translate(glm::vec3 speedVector) {
		position += speedVector;
		(*model).setTranslationMatrix(glm::translate(glm::mat4(1.0f), position));
	}

	GameObject::~GameObject() {
		delete model;
	}

}