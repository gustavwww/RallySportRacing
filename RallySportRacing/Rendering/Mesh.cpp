#include "Mesh.h"
#include <glm/glm.hpp>
#include <GL/glew.h>

namespace Rendering {

	Mesh::Mesh(vector<SubMesh*> subMeshes, string name) {
		this->subMeshes = subMeshes;
		this->name = name;
	}

	void Mesh::renderMesh(GLint programID) {
		for each (SubMesh* sm in subMeshes) {
			sm->renderSubMesh(programID);
		}
	}
}