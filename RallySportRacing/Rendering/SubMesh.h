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
		glm::vec2 texCoord;
	};

	struct Material {

		//ToDo remove old values.
		glm::vec3 albedo;
		float metallic;
		float roughness;
		unsigned int baseColorTexture;
		unsigned int ambientOcclusionTexture;
		unsigned int roughnessTexture;
		unsigned int metallicTexture;
	};

	class SubMesh {
	public:
		SubMesh(vector<Vertex> vertices, vector<unsigned int> indices, Material material);
		void renderSubMesh(GLint programID);
	private:
		vector<Vertex> vertices;
		vector<unsigned int> indices;

		Material material;
		
		unsigned int VAO, VBO, EBO;

		bool isMeshSetup = false;
		void setupSubMesh();
	
	};
}

#endif