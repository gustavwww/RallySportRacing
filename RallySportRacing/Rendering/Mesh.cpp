#include "Mesh.h"
#include <glm/glm.hpp>
#include <GL/glew.h>

#include "../../External/bullet/headers/btBulletDynamicsCommon.h"

namespace Rendering {



	Mesh::Mesh(vector<SubMesh> subMeshes) {
		this->subMeshes = subMeshes;
	}

	void Mesh::renderMesh(GLint programID) {
		for each (SubMesh sm in subMeshes) {
			sm.renderSubMesh(programID);
		}
	}
}