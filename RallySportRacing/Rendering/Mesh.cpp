#include "Mesh.h"
#include <glm/glm.hpp>
#include <GL/glew.h>

#include <btBulletDynamicsCommon.h>

namespace Rendering {

	Mesh::Mesh(vector<SubMesh*> subMeshes, string name, unsigned int instances, vector<glm::mat4> instanceMatrices) {
		this->subMeshes = subMeshes;
		this->name = name;
		this->instances = instances;
		this->instanceMatrices = instanceMatrices;
	}

	void Mesh::multiplyMatrixRelativeWithModel(glm::mat4 modelMatrix) {
		for each (glm::mat4 instanceMatrix in instanceMatrices) {
			instanceWorldMatrices.push_back(modelMatrix * instanceMatrix);
		}
	}

	void Mesh::renderMesh(GLint programID, glm::mat4 modelMatrix, glm::mat4 projection, glm::mat4 view, bool isMovable) {
		if (isMovable == true) {
			instanceWorldMatrices.clear();
			multiplyMatrixRelativeWithModel(modelMatrix);
		}
		else if (isFirst == true) {
			multiplyMatrixRelativeWithModel(modelMatrix);
			isFirst = false;
		}
		glm::mat4 viewInverse = inverse(view);

		//Send matrix to shader
		GLuint matrixID = glGetUniformLocation(programID, "view");
		glUniformMatrix4fv(matrixID, 1, GL_FALSE, &view[0][0]);

		GLuint modelViewMatrixID = glGetUniformLocation(programID, "projection");
		glUniformMatrix4fv(modelViewMatrixID, 1, GL_FALSE, &projection[0][0]);

		GLuint viewInverseMatrixID = glGetUniformLocation(programID, "viewInverse");
		glUniformMatrix4fv(viewInverseMatrixID, 1, GL_FALSE, &viewInverse[0][0]);

		for each (SubMesh * sm in subMeshes) {
			sm->renderSubMesh(programID, instances, instanceWorldMatrices, isMovable);
		}
	}
}