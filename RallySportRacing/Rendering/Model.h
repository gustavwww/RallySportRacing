#ifndef MODEL
#define MODEL

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include "Mesh.h"
//#include <headers/btBulletDynamicsCommon.h>
#include <btBulletDynamicsCommon.h>

using namespace std;

namespace Rendering {

	class Model {
	public:

		vector<Mesh> meshes;
		vector<Material> materials;
		bool isMovable = false;
		string modelName;

		Model(vector<Mesh> meshes, vector<Material> materials, bool isMovable, string modelName);
		~Model();

		void updateMaterial(int colorIndex, string materialName);
		void setTranslationMatrix(glm::mat4 translationMat);
		void setScaleMatrix(glm::mat4 scaleMat);
		void setRotationMatrix(glm::mat4 rotationMat);
		
		void render(glm::mat4 projection, glm::mat4 view, GLint programID);

		btVector3 Model::generateCollisionShape();
		btVector3 Model::generateCollisionShapeOffset();

		static Model* loadModel(const char* file, bool isTerrain, bool isMovable);
		static unsigned int loadModelTexture(const char* filePath, GLint channels);
		static void swapModelTexture(const char* filePath, GLint channels, unsigned int textureID);

		btTriangleMesh* getMeshInterface();

	private:
		glm::mat4 translationMat = glm::mat4(1.0f);
		glm::mat4 scaleMat = glm::mat4(1.0f);
		glm::mat4 rotationMat = glm::mat4(1.0f);
		vector<const char*> colors {"../Models/Textures/Blue.png", "../Models/Textures/Red.png", "../Models/Textures/Green.png", 
			"../Models/Textures/Orange.png", "../Models/Textures/Purple.png", "../Models/Textures/Grey.jpg"
		};
	};
}

#endif
