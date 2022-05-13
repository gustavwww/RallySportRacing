#ifndef MESH
#define MESH

#include <vector>
#include <string>
#include "SubMesh.h"

using namespace std;

namespace Rendering {

	class Mesh {
	public:
		string name;
		vector<SubMesh*> subMeshes;
		unsigned int instances;
		vector<glm::mat4> instanceMatrices;
		vector<glm::mat4> instanceWorldMatrices;

		Mesh(vector<SubMesh*> subMeshes, string name, unsigned int instances, vector<glm::mat4> instanceMatrices);

		void renderMesh(GLint programID, glm::mat4 modelMatrix, glm::mat4 projection, glm::mat4 view, bool isMovable);

	private:
		void multiplyMatrixRelativeWithModel(glm::mat4 modelMatrix);
		bool isFirst = true;
	};
}

#endif