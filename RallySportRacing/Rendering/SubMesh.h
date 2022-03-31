#ifndef SUBMESH
#define SUBMESH

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

using namespace std;

namespace Rendering {

	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
	};

	class SubMesh {
	public:
		SubMesh(vector<Vertex> vertices, vector<unsigned int> indices, float roughness, float metallic, glm::vec3 albedo);
		void renderSubMesh(GLint programID);
	private:
		vector<Vertex> vertices;
		vector<unsigned int> indices;

		float roughness;
		float metallic;
		glm::vec3 albedo;
		
		unsigned int VAO, VBO, EBO;

		bool isMeshSetup = false;
		void setupSubMesh();
	
	};
}

#endif