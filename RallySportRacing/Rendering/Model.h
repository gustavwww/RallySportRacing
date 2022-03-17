#ifndef MODEL
#define MODEL

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include "Mesh.h"

using namespace std;

namespace Rendering {

	struct Material {
		glm::vec3 albedo;
		float metallic;
		float roughness;
	};

	class Model {
	public:
		vector<Mesh> meshes;

		Model(vector<Mesh> meshes);
		~Model();

		void setTranslationMatrix(glm::mat4 translationMat);
		void setScaleMatrix(glm::mat4 scaleMat);
		void setRotationMatrix(glm::mat4 rotationMat);
		
		void render(glm::mat4 projection, glm::mat4 view, GLint programID);

		static Model* loadModel(const char* file);

	private:
		glm::mat4 translationMat = glm::mat4(1.0f);
		glm::mat4 scaleMat = glm::mat4(1.0f);
		glm::mat4 rotationMat = glm::mat4(1.0f);
	};
}

#endif
