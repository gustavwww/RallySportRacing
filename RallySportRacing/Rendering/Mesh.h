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
		vector<SubMesh> subMeshes;

		Mesh(vector<SubMesh> subMeshes, string name);

		void renderMesh(GLint programID);

	private:
		
	};
}

#endif