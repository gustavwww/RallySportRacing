#include "SubMesh.h"

namespace Rendering {

	SubMesh::SubMesh(vector<Vertex> vertices, vector<unsigned int> indices, Material material) {
		this->vertices = vertices;
		this->indices = indices;
		this->material = material;
	}

	void SubMesh::setupSubMesh() {

		vector<glm::vec3> normals;
		vector<glm::vec3> positions;
		vector<glm::vec2> texCoords;

		for each (Vertex vertex in vertices) {
			normals.push_back(vertex.normal);
			positions.push_back(vertex.position);
			texCoords.push_back(vertex.texCoord);
		}
		
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		//Indices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		//Positions
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		//Normals
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

		//TexCoords
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
	}
	
	void SubMesh::renderSubMesh(GLint programID) {
		if (!isMeshSetup) {
			setupSubMesh();
			isMeshSetup = true;
		}
		//Textures
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, material.baseColorTexture);
		GLuint baseColorTexID = glGetUniformLocation(programID, "baseColorTexture");
		glUniform1i(baseColorTexID, 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, material.metallicTexture);
		GLuint metallicTexID = glGetUniformLocation(programID, "metallicTexture");
		glUniform1i(metallicTexID, 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, material.roughnessTexture);
		GLuint roughnessTexID = glGetUniformLocation(programID, "roughnessTexture");
		glUniform1i(roughnessTexID, 3);



		//Int 
		GLuint baseColorIntID = glGetUniformLocation(programID, "useBaseColorTexture");
		glUniform1ui(baseColorIntID, material.baseColorTexture);

		GLuint metallicIntID = glGetUniformLocation(programID, "useMetallicTexture");
		glUniform1ui(metallicIntID, material.metallicTexture);

		GLuint roughnessIntID = glGetUniformLocation(programID, "useRoughnessTexture");
		glUniform1ui(roughnessIntID, material.roughnessTexture);



		//Material Values.
		GLuint roughnessID = glGetUniformLocation(programID, "roughnessValue");
		glUniform1fv(roughnessID, 1, &material.roughness);
		
		GLuint metallicID = glGetUniformLocation(programID, "metallicValue");
		glUniform1fv(metallicID, 1, &material.metallic);

		GLuint albedoID = glGetUniformLocation(programID, "albedoValue");
		glUniform3fv(albedoID, 1, &material.albedo[0]);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);
	}
}