#ifndef GAMEOBJECT
#define GAMEOBJECT

#include "Rendering/Model.h"
#include <glm/glm.hpp>

namespace Game {

	class GameObject {
	public:
		GameObject(Rendering::Model* model);
		~GameObject();

		void translate(glm::vec3 speedVector);
		void rotate(glm::vec3 angleVector);

		glm::vec3 getPosition();
		glm::vec3 getOrientation();
	private:
		Rendering::Model* model;
		glm::vec3 position;
		glm::vec3 orientation;

	};

}




#endif
