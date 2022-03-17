#include "SubMesh.h"

namespace Rendering {

	SubMesh::SubMesh(vector<Vertex> vertices, vector<unsigned int> indices, float roughness, float metallic, glm::vec3 albedo) {
		this->vertices = vertices;
		this->indices = indices;
		this->roughness = roughness;
		this->metallic = metallic;
		this->albedo = albedo;

		setupSubMesh();
	}

	void SubMesh::setupSubMesh() {
		vector<glm::vec3> normals;
		vector<glm::vec3> positions;

		for each (Vertex vertex in vertices) {
			normals.push_back(vertex.normal);
			positions.push_back(vertex.position);
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
	}

	void SubMesh::renderSubMesh(GLint programID) {
		
		//Send materials to shader.
		GLuint roughnessID = glGetUniformLocation(programID, "roughness");
		glUniform1fv(roughnessID, 1, &roughness);
		
		GLuint metallicID = glGetUniformLocation(programID, "metallic");
		glUniform1fv(metallicID, 1, &metallic);

		GLuint albedoID = glGetUniformLocation(programID, "albedo");
		glUniform3fv(albedoID, 1, &albedo[0]);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);
	}
}