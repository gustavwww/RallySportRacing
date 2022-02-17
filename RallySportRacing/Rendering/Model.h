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
		void render();
		static Model* createCube();

	private:
		vector<glm::vec3> verticies;
		vector<glm::vec3> colors;
		GLuint vertexArrayID;
		GLuint vertexBuffer;
		GLuint colorBuffer;
	};

}

#endif
