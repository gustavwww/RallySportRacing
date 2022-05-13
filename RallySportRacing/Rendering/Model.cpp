#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "Model.h"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
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

	Model::Model(vector<Mesh> meshes, vector<Material> materials, bool isMovable, string modelName) {
		this->meshes = meshes;
		this->materials = materials;
		this->isMovable = isMovable;
		this->modelName = modelName;
	}

	Model::~Model() {

		//glDeleteBuffers(1, &vertexBuffer);
		//glDeleteBuffers(1, &colorBuffer);
		//glDeleteVertexArrays(1, &vertexArrayID);
	}

	void Model::updateMaterial(const char* textureFilePath, string materialName) {
		for each (Material material in materials) {
			if (material.materialName == materialName) {
				swapModelTexture(textureFilePath, GL_RGBA, material.baseColorTexture);
			}
		}
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

		glm::mat4 model = translationMat * rotationMat * scaleMat;

		for each (Mesh m in meshes) {
			m.renderMesh(programID, model, projection, view, isMovable);
		}
	}

	void Model::swapModelTexture(const char* filePath, GLint channels, unsigned int textureID) {

		ifstream textureStream(filePath, ios::in);
		if (!textureStream.is_open()) {
			printf("Could not open %s. Maybe in the wrong directory?\n", filePath);
		}
		else {

			int width, height, nrChannels;
			unsigned char* image = stbi_load(filePath, &width, &height, &nrChannels, STBI_rgb_alpha);

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

	Model* Model::loadModel(const char* file, bool isTerrain, bool isMovable) {
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

		unsigned int meshNumber = 0;
		
		bool res = loader.LoadASCIIFromFile(&gltfmodel, &err, &warn, file);

		//Temp variable.
		

		//Load materials.
		for each (tinygltf::Material gltfMaterial in gltfmodel.materials) {
			Material material = Material();

			material.materialName = gltfMaterial.name;

			material.metallic = gltfMaterial.pbrMetallicRoughness.metallicFactor;
			material.roughness = gltfMaterial.pbrMetallicRoughness.roughnessFactor;
			material.albedo = glm::vec3(gltfMaterial.pbrMetallicRoughness.baseColorFactor[0], gltfMaterial.pbrMetallicRoughness.baseColorFactor[1], gltfMaterial.pbrMetallicRoughness.baseColorFactor[2]);
		
			//Basecolor texture
			int baseColorTextureIndex = gltfMaterial.pbrMetallicRoughness.baseColorTexture.index;
			//If no texture found.
			if (baseColorTextureIndex != -1) {
				string path = "../Models/" + gltfmodel.images[baseColorTextureIndex].uri;
				const char* baseColorTexturePath = path.c_str();
				material.baseColorTexture = loadModelTexture(baseColorTexturePath, GL_RGBA);
			}
			
			//ToDo finish loading textures.
			//Metallic roughness texture
			int metallicRoughnessTextureIndex = gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index;
			//If no texture found.
			if (metallicRoughnessTextureIndex != -1) {
				string path = "../Models/" + gltfmodel.images[metallicRoughnessTextureIndex].uri;
				const char* metallicRoughnessTexturePath = path.c_str();
				material.ambientOcclusionTexture = loadModelTexture(metallicRoughnessTexturePath, GL_RED);
				material.roughnessTexture = loadModelTexture(metallicRoughnessTexturePath, GL_GREEN);
				material.metallicTexture = loadModelTexture(metallicRoughnessTexturePath, GL_BLUE);
			}
			
			materials.push_back(material);
		}

		for each (tinygltf::Mesh gltfMesh in gltfmodel.meshes) {

			vector<glm::mat4> instanceMatrices;
			unsigned int instances = 0;

			for each (tinygltf::Node gltfNode in gltfmodel.nodes) {

				if (gltfNode.mesh == meshNumber) {
					glm::mat4 relativeTranslation = glm::mat4(1.0f);
					glm::mat4 scale = glm::mat4(1.0f);
					glm::mat4 rotation = glm::mat4(1.0f);

					if (size(gltfNode.translation) == 3) {
						relativeTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(gltfNode.translation[0], gltfNode.translation[1], gltfNode.translation[2]));
					}
					if (size(gltfNode.scale) == 3) {
						scale = glm::scale(glm::mat4(1.0f), glm::vec3(gltfNode.scale[0], gltfNode.scale[1], gltfNode.scale[2]));
					}
					if (size(gltfNode.rotation) == 4) {
						rotation = glm::toMat4(glm::quat(gltfNode.rotation[3], gltfNode.rotation[0], gltfNode.rotation[1], gltfNode.rotation[2]));
					}
					instances++;
					instanceMatrices.push_back(relativeTranslation * rotation * scale);
				}
			}

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

				//TexCoords accessor.
				const tinygltf::Accessor& accessorTexCoords = gltfmodel.accessors[primitive.attributes["TEXCOORD_0"]];
				const tinygltf::BufferView& bufferViewTexCoords = gltfmodel.bufferViews[accessorTexCoords.bufferView];
				const tinygltf::Buffer& bufferTexCoords = gltfmodel.buffers[bufferViewTexCoords.buffer];
				const float* texCoords = reinterpret_cast<const float*>(&bufferTexCoords.data[bufferViewTexCoords.byteOffset + accessorTexCoords.byteOffset]);

				const float* texCoords2;
				if (gltfMesh.name == "tree.007" || gltfMesh.name == "tree.001") {
					const tinygltf::Accessor& accessorTexCoords = gltfmodel.accessors[primitive.attributes["TEXCOORD_1"]];
					const tinygltf::BufferView& bufferViewTexCoords = gltfmodel.bufferViews[accessorTexCoords.bufferView];
					const tinygltf::Buffer& bufferTexCoords = gltfmodel.buffers[bufferViewTexCoords.buffer];
					texCoords2 = reinterpret_cast<const float*>(&bufferTexCoords.data[bufferViewTexCoords.byteOffset + accessorTexCoords.byteOffset]);
				}
				else {
					texCoords2 = texCoords;
				}

				//Contructing vertices.
				for (size_t i = 0; i < accessorPositions.count; i++) {
					vertex.position = glm::vec3(positions[i * 3 + 0], positions[i * 3 + 1], positions[i * 3 + 2]);
					vertex.normal = glm::vec3(normals[i * 3 + 0], normals[i * 3 + 1], normals[i * 3 + 2]);
					vertex.texCoord = glm::vec2(texCoords[i * 2 + 0], texCoords[i * 2 + 1]);
					vertex.texCoord2 = glm::vec2(texCoords2[i * 2 + 0], texCoords2[i * 2 + 1]);
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
				subMeshes.push_back(new SubMesh(vertices, indices, materials[primitive.material]));
			}
			
			meshes.push_back(Mesh(subMeshes, gltfMesh.name, instances, instanceMatrices));
			meshNumber++;
		}
		Model* model = new Model(meshes, materials, isMovable, file);

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