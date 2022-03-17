#ifndef MESH
#define MESH

#include <vector>
#include "SubMesh.h"

using namespace std;

namespace Rendering {

	class Mesh {
	public:
		vector<SubMesh> subMeshes;

		Mesh(vector<SubMesh> subMeshes);

		void renderMesh(GLint programID);

	private:
		
	};
}

#endif