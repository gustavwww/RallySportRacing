#ifndef MODEL
#define MODEL

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>

using namespace std;

namespace Rendering {

	class Model {
	public:
		Model(vector<glm::vec3> verticies, vector<glm::vec3> colors);
		~Model();

		void setTranslationMatrix(glm::mat4 translationMat);
		void setScaleMatrix(glm::mat4 scaleMat);
		void setRotationMatrix(glm::mat4 rotationMat);
		void render(GLuint matrixID, GLuint modelViewMatrixID, GLuint normalMatrixID, glm::mat4 projection, glm::mat4 view);

		static Model* createCube();

	private:
		glm::mat4 translationMat = glm::mat4(1.0f);
		glm::mat4 scaleMat = glm::mat4(1.0f);
		glm::mat4 rotationMat = glm::mat4(1.0f);

		vector<glm::vec3> verticies;
		vector<glm::vec3> colors;
		GLuint vertexArrayID;
		GLuint vertexBuffer;
		GLuint colorBuffer;
	};

}

#endif
