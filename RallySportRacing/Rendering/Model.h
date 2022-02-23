#ifndef MODEL
#define MODEL

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
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
		//void setViewMatrix(glm::mat4 viewMat);
		void render(GLuint matrixID, glm::mat4 projection, glm::mat4 view);

		static Model* createCube();

	private:
		glm::mat4 translationMat = glm::mat4(1.0f);
		glm::mat4 scaleMat = glm::mat4(1.0f);
		glm::mat4 rotationMat = glm::mat4(1.0f);
		// Params: Cam pos in World Space, where to look at, head up (0,-1,0) = upside down.
		//glm::mat4 viewMat = glm::lookAt(glm::vec3(0, 5, 20), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); //start values

		vector<glm::vec3> verticies;
		vector<glm::vec3> colors;
		GLuint vertexArrayID;
		GLuint vertexBuffer;
		GLuint colorBuffer;
	};

}

#endif
