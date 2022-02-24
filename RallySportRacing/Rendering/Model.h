#ifndef MODEL
#define MODEL

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>

using namespace std;

namespace Rendering {

	struct Vertex {
		glm::vec3 positions;
		glm::vec2 texUVs;
		glm::vec3 normals;
	};

	class Model {
	public:
		Model(vector<glm::vec3> vertices, vector<glm::vec3> colors, vector<unsigned int> indices, vector<glm::vec3> normals);
		~Model();

		void setTranslationMatrix(glm::mat4 translationMat);
		void setScaleMatrix(glm::mat4 scaleMat);
		void setRotationMatrix(glm::mat4 rotationMat);
		
		void render(GLuint matrixID, GLuint modelViewMatrixID, GLuint normalMatrixID, glm::mat4 projection, glm::mat4 view);


		static Model* createModel(const char* file);
		static Model* createCube();

	private:
		glm::mat4 translationMat = glm::mat4(1.0f);
		glm::mat4 scaleMat = glm::mat4(1.0f);
		glm::mat4 rotationMat = glm::mat4(1.0f);

		vector<glm::vec3> vertices;
		vector<glm::vec3> colors;
		vector<unsigned int> indices;
		vector<glm::vec3> normals;
		GLuint vertexArrayID;
		GLuint vertexBuffer;
		GLuint colorBuffer;
		GLuint indexBuffer;
		GLuint normalBuffer;
		/*
		const char* file;
		std::vector<unsigned char> data;
		json JSON;

		std::vector<unsigned char> getData();
		std::vector<float> getFloats(json accessor);
		std::vector<GLuint> getIndices(json accessor);

		std::vector<Vertex> assembleVertices(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<glm::vec2> texUVs);

		std::vector<glm::vec2> groupFloatsVec2(std::vector<float> floatVec);
		std::vector<glm::vec3> groupFloatsVec3(std::vector<float> floatVec);
		std::vector<glm::vec4> groupFloatsVec4(std::vector<float> floatVec);

		*/
	};
}

#endif
