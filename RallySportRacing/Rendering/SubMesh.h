#ifndef SUBMESH
#define SUBMESH

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>
#include <string>

using namespace std;

namespace Rendering {

	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoord;
		glm::vec2 texCoord2;
	};

	struct Material {

		//ToDo remove old values.
		string materialName;
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
		void renderSubMesh(GLint programID, unsigned int instances, vector<glm::mat4> instanceWorldMatrices, bool isMovable);
		void setTransformMatrices(vector<glm::mat4> instanceWorldMatrices);
	private:
		vector<Vertex> vertices;
		vector<unsigned int> indices;

		Material material;
		
		unsigned int VAO, VBO, instanceVBO, EBO;

		bool isMeshSetup = false;
		void setupSubMesh(vector<glm::mat4> instanceWorldMatrices, bool isMovable);
	
	};
}

#endif