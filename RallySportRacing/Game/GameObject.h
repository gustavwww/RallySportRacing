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
	private:
		Rendering::Model* model;
		glm::vec3 position;

	};

}




#endif
