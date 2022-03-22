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

#include "btBulletDynamicsCommon.h"

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

	Model* Model::loadModel(const char* file) {
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
			materials.push_back(material);
		}

		for each (tinygltf::Mesh gltfMesh in gltfmodel.meshes) {

			cout << gltfMesh.name;

			//Mesh data vector.
			vector<SubMesh> subMeshes;

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

				std::vector <glm::vec3> face;

				//Contructing vertices.
				for (size_t i = 0; i < accessorPositions.count; i++) {
					vertex.position = glm::vec3(positions[i * 3 + 0], positions[i * 3 + 1], positions[i * 3 + 2]);
					vertex.normal = glm::vec3(normals[i * 3 + 0], normals[i * 3 + 1], normals[i * 3 + 2]);
					vertices.push_back(vertex);
					face.push_back(glm::vec3(vertex.position.x, vertex.position.y, vertex.position.z));
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
				

				

				//Add SubMesh to Mesh.
				subMeshes.push_back(SubMesh(vertices, indices, materials[primitive.material].roughness, materials[primitive.material].metallic, materials[primitive.material].albedo));
			}

			meshes.push_back(Mesh(subMeshes));
		}

		Model* model = new Model(meshes);

		return model;
	}

	void Model::generateMeshInterface() {
		meshInterface = new btTriangleMesh();

		for (int i = 0; i < faces.size(); i++) {
			std::vector<glm::vec3> face = faces[i];
			meshInterface->addTriangle(
				btVector3(face[0].x, face[0].y, face[0].z),
				btVector3(face[1].x, face[1].y, face[1].z),
				btVector3(face[2].x, face[2].y, face[2].z)
			);
		}
	}
}