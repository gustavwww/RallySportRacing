#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "Model.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <tiny_gltf.h>

#include <btBulletDynamicsCommon.h>

#include "Mesh.h";
#include <iostream>
using namespace std;
using namespace tinygltf;

namespace Rendering {

	Model::Model(vector<Mesh> meshes) {
		this->meshes = meshes;
	}

	Model::~Model() {

		//glDeleteBuffers(1, &vertexBuffer);
		//glDeleteBuffers(1, &colorBuffer);
		//glDeleteVertexArrays(1, &vertexArrayID);
	}

	void Model::setTranslationMatrix(glm::mat4 translationMat) {
		this->translationMat = translationMat;
	}

	void Model::setScaleMatrix(glm::mat4 scaleMat) {
		this->scaleMat = scaleMat;
	}

	void Model::setRotationMatrix(glm::mat4 rotationMat) {
		this->rotationMat = rotationMat;
	}


	void Model::render(glm::mat4 projection, glm::mat4 view, GLint programID) {

		// Model matrix: TranslationMatrix * RotationMatrix * ScaleMatrix * OriginalVector
		glm::mat4 model = translationMat * rotationMat * scaleMat;

		glm::mat4 modelViewMatrix = view * model;
		glm::mat4 mvp = projection * modelViewMatrix;
		glm::mat4 normalMatrix = inverse(transpose(modelViewMatrix));

		//Send matrix to shader
		GLuint matrixID = glGetUniformLocation(programID, "MVP");
		glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);

		GLuint modelViewMatrixID = glGetUniformLocation(programID, "modelViewMatrix");
		glUniformMatrix4fv(modelViewMatrixID, 1, GL_FALSE, &modelViewMatrix[0][0]);
		
		GLuint normalMatrixID = glGetUniformLocation(programID, "normalMatrix");
		glUniformMatrix4fv(normalMatrixID, 1, GL_FALSE, &normalMatrix[0][0]);

		for each (Mesh m in meshes) {
			m.renderMesh(programID);
		}
	}
	
	//ToDo finish method.
	unsigned int Model::loadModelTexture(const char* filePath, GLint channels) {
		
		ifstream textureStream(filePath, ios::in);
		if (!textureStream.is_open()) {
			printf("Could not open %s. Maybe in the wrong directory?\n", filePath);
			return 0;
		}

		int width, height, nrChannels;
		unsigned char* image = stbi_load(filePath, &width, &height, &nrChannels, STBI_rgb_alpha);
		unsigned int textureID;

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, channels, width, height, 0, channels, GL_UNSIGNED_BYTE, image);
		free(image);

		//Set wrapping type.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//Mipmap and filtering.
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);

		return textureID;
	}
	
	
	double maxX = 0;
	double maxY = 0;
	double maxZ = 0;
	double minX = 0;
	double minY = 0;
	double minZ = 0;

	btTriangleMesh* meshInterface = new btTriangleMesh();

	Model* Model::loadModel(const char* file, bool isTerrain) {
		maxX = 0;
		maxY = 0;
		maxZ = 0;
		minX = 0;
		minY = 0;
		minZ = 0;
		meshInterface = new btTriangleMesh();

		tinygltf::Model gltfmodel;
		TinyGLTF loader;
		std::string err;
		std::string warn;

		//Model data vector.
		vector<Rendering::Mesh> meshes;
		vector<Material> materials;
		
		bool res = loader.LoadASCIIFromFile(&gltfmodel, &err, &warn, file);

		//Temp variable.
		Material material;

		//Load materials.
		for each (tinygltf::Material gltfMaterial in gltfmodel.materials) {
			material.metallic = gltfMaterial.pbrMetallicRoughness.metallicFactor;
			material.roughness = gltfMaterial.pbrMetallicRoughness.roughnessFactor;
			material.albedo = glm::vec3(gltfMaterial.pbrMetallicRoughness.baseColorFactor[0], gltfMaterial.pbrMetallicRoughness.baseColorFactor[1], gltfMaterial.pbrMetallicRoughness.baseColorFactor[2]);
		
			//Basecolor texture
			int baseColorTextureIndex = gltfMaterial.pbrMetallicRoughness.baseColorTexture.index;
			const char* baseColorTexturePath = gltfmodel.images[baseColorTextureIndex].uri.c_str();
			material.baseColorTexture = loadModelTexture(baseColorTexturePath, GL_RGBA);

			//ToDo finish loading textures.
			//Metallic roughness texture
			int metallicRoughnessTextureIndex = gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index;
			const char* metallicRoughnessTexturePath = gltfmodel.images[metallicRoughnessTextureIndex].uri.c_str();
			material.ambientOcclusionTexture = loadModelTexture(metallicRoughnessTexturePath, GL_RED);
			material.roughnessTexture = loadModelTexture(metallicRoughnessTexturePath, GL_GREEN);
			material.metallicTexture = loadModelTexture(metallicRoughnessTexturePath, GL_BLUE);
			

			materials.push_back(material);
		}

		for each (tinygltf::Mesh gltfMesh in gltfmodel.meshes) {


			//Mesh data vector.
			vector<SubMesh*> subMeshes;

			for each (Primitive primitive in gltfMesh.primitives) {

				//SubMesh data vectors.
				vector<Vertex> vertices;
				vector<unsigned int> indices;

				//Temp variable
				Vertex vertex;
				//Positions accessor.
				const tinygltf::Accessor& accessorPositions = gltfmodel.accessors[primitive.attributes["POSITION"]];
				const tinygltf::BufferView& bufferViewPositions = gltfmodel.bufferViews[accessorPositions.bufferView];
				const tinygltf::Buffer& bufferPositions = gltfmodel.buffers[bufferViewPositions.buffer];
				const float* positions = reinterpret_cast<const float*>(&bufferPositions.data[bufferViewPositions.byteOffset + accessorPositions.byteOffset]);

				//Normals accessor.
				const tinygltf::Accessor& accessorNormals = gltfmodel.accessors[primitive.attributes["NORMAL"]];
				const tinygltf::BufferView& bufferViewNormals = gltfmodel.bufferViews[accessorNormals.bufferView];
				const tinygltf::Buffer& bufferNormals = gltfmodel.buffers[bufferViewNormals.buffer];
				const float* normals = reinterpret_cast<const float*>(&bufferNormals.data[bufferViewNormals.byteOffset + accessorNormals.byteOffset]);


				//Contructing vertices.
				for (size_t i = 0; i < accessorPositions.count; i++) {
					vertex.position = glm::vec3(positions[i * 3 + 0], positions[i * 3 + 1], positions[i * 3 + 2]);
					vertex.normal = glm::vec3(normals[i * 3 + 0], normals[i * 3 + 1], normals[i * 3 + 2]);
					vertices.push_back(vertex);

					if (maxX <= vertex.position.x) {
						maxX = vertex.position.x;
					}
					if (maxY <= vertex.position.y) {
						maxY = vertex.position.y;
					}
					if (maxZ <= vertex.position.z) {
						maxZ = vertex.position.z;
					}

					if (minX >= vertex.position.x) {
						minX = vertex.position.x;
					}
					if (minY >= vertex.position.y) {
						minY = vertex.position.y;
					}
					if (minZ >= vertex.position.z) {
						minZ = vertex.position.z;
					}
				}

				//Indices accessor.
				const tinygltf::Accessor& accessorIndices = gltfmodel.accessors[primitive.indices];
				const tinygltf::BufferView& bufferViewIndices = gltfmodel.bufferViews[accessorIndices.bufferView];
				const tinygltf::Buffer& bufferIndices = gltfmodel.buffers[bufferViewIndices.buffer];
				if (accessorIndices.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
					const unsigned short* indicesgltf = reinterpret_cast<const unsigned short*>(&bufferIndices.data[bufferViewIndices.byteOffset + accessorIndices.byteOffset]);
					for (size_t i = 0; i < accessorIndices.count; i++) {
						indices.push_back(unsigned int(indicesgltf[i]));
					}
				}
				else if (accessorIndices.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
					const unsigned int* indicesgltf = reinterpret_cast<const unsigned int*>(&bufferIndices.data[bufferViewIndices.byteOffset + accessorIndices.byteOffset]);
					for (size_t i = 0; i < accessorIndices.count; i++) {
						indices.push_back(unsigned int(indicesgltf[i]));
					}
				}

				// Calculates a meshinterface of the model if it is a terrain
				if (isTerrain) {
					for (int i = 0; i < accessorIndices.count; i++) {
						if ((i + 1) % 3 == 0) {
							meshInterface->addTriangle(
								btVector3(vertices[indices[i-2]].position.x, vertices[indices[i - 2]].position.y, vertices[indices[i - 2]].position.z),
								btVector3(vertices[indices[i - 1]].position.x, vertices[indices[i - 1]].position.y, vertices[indices[i - 1]].position.z),
								btVector3(vertices[indices[i]].position.x, vertices[indices[i]].position.y, vertices[indices[i]].position.z)
							);
						}
					}
				}

				

				//Add SubMesh to Mesh.
				subMeshes.push_back(new SubMesh(vertices, indices, materials[primitive.material].roughness, materials[primitive.material].metallic, materials[primitive.material].albedo));
			}
			
			meshes.push_back(Mesh(subMeshes, gltfMesh.name));

		}
		Model* model = new Model(meshes);

		return model;
	}

	// Gives the meshinterface for terrainmodels. 
	btTriangleMesh* Model::getMeshInterface()
	{
		return meshInterface;
	}

	// Gives outer bounds shape of the model
	btVector3 Model::generateCollisionShape() {
		return btVector3((maxX-minX)/2, (maxY - minY)/2, (maxZ - minZ) / 2);
	}

	// Gives centerpoint of the model
	btVector3 Model::generateCollisionShapeOffset() {
		return btVector3((maxX + minX) / 2, (maxY + minY) / 2, (maxZ + minZ) / 2);
	}


}