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

using namespace std;
using namespace tinygltf;

namespace Rendering {

	Model::Model(vector<glm::vec3> vertices, vector<glm::vec3> colors, vector<unsigned int> indices, vector<glm::vec3> normals) {

		this->vertices = vertices;
		this->colors = colors;
		this->indices = indices;
		this->normals = normals;

		/*std::string text = get_file_contents(file);
		JSON = json::parse(text);

		Model::file = file;
		data = getData();*/

		glGenVertexArrays(1, &vertexArrayID);
		glBindVertexArray(vertexArrayID);

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(glm::vec3), &this->vertices[0].x, GL_STATIC_DRAW);

		glGenBuffers(1, &colorBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
		glBufferData(GL_ARRAY_BUFFER, this->colors.size() * sizeof(glm::vec3), &this->colors[0].x, GL_STATIC_DRAW);

		glGenBuffers(1, &indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), &this->indices[0], GL_STATIC_DRAW);

		glGenBuffers(1, &normalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
		glBufferData(GL_ARRAY_BUFFER, this->normals.size() * sizeof(glm::vec3), &this->normals[0].x, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		// Pointer to which attribute to fill in vertex shader. (location=0)
		// Params: Attribute ID, size (x,y,z), type, normalized?, stride, array buffer offset.
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		
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


	void Model::render(GLuint matrixID, GLuint modelViewMatrixID, GLuint normalMatrixID, glm::mat4 projection, glm::mat4 view) {

		// Model matrix: TranslationMatrix * RotationMatrix * ScaleMatrix * OriginalVector
		glm::mat4 model = translationMat * rotationMat * scaleMat;
		
		glm::mat4 modelViewMatrix = view * model;
		glm::mat4 mvp = projection * modelViewMatrix;
		glm::mat4 normalMatrix = inverse(transpose(modelViewMatrix));


		// Send matrix to shader
		glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);
		glUniformMatrix4fv(modelViewMatrixID, 1, GL_FALSE, &modelViewMatrix[0][0]);
		glUniformMatrix4fv(normalMatrixID, 1, GL_FALSE, &normalMatrix[0][0]);

		glBindVertexArray(vertexArrayID);

		//glDrawArrays(GL_TRIANGLES, 0, this->vertices.size() * 3);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);
	}

	Model* Model::createModel(const char* file) {

		tinygltf::Model gltfmodel;
		TinyGLTF loader;
		std::string err;
		std::string warn;
		vector<glm::vec3> vertices;
		vector<glm::vec3> colors;
		vector<unsigned int> indices;
		vector<glm::vec3> normals;
		glm::vec3 red = glm::vec3(1.0f, 0.f, 0.f);
		glm::vec3 blue = glm::vec3(0.f, 0.f, 1.f);
		glm::vec3 green = glm::vec3(0.f, 1.f, 0.f);
		glm::vec3 colorSelection = red;
		
		bool res = loader.LoadASCIIFromFile(&gltfmodel, &err, &warn, file);
		unsigned int offset = 0;
		for each (Mesh mesh in gltfmodel.meshes) {
			for each (Primitive primitive in mesh.primitives) {
				{
					const tinygltf::Accessor& accessor = gltfmodel.accessors[primitive.attributes["POSITION"]];
					const tinygltf::BufferView& bufferView = gltfmodel.bufferViews[accessor.bufferView];
					const tinygltf::Buffer& buffer = gltfmodel.buffers[bufferView.buffer];
					const float* positions = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

					for (size_t i = 0; i < accessor.count; i++) {
						vertices.push_back(glm::vec3(positions[i * 3 + 0], positions[i * 3 + 1], positions[i * 3 + 2]));
					
						if (colorSelection == red) {
							colors.push_back(colorSelection);
							colorSelection = blue;
						}
						else if (colorSelection == blue) {
							colors.push_back(colorSelection);
							colorSelection = green;
						}
						else {
							colors.push_back(colorSelection);
							colorSelection = red;
						}
					}
				}
				{
					const tinygltf::Accessor& accessor = gltfmodel.accessors[primitive.indices];
					const tinygltf::BufferView& bufferView = gltfmodel.bufferViews[accessor.bufferView];
					const tinygltf::Buffer& buffer = gltfmodel.buffers[bufferView.buffer];
					const unsigned short* indicesgltf = reinterpret_cast<const unsigned short*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

					for (size_t i = 0; i < accessor.count; i++) {
						indices.push_back(offset + unsigned int(indicesgltf[i]));
					}
				}
				offset = vertices.size();
				{
					const tinygltf::Accessor& accessor = gltfmodel.accessors[primitive.attributes["NORMAL"]];
					const tinygltf::BufferView& bufferView = gltfmodel.bufferViews[accessor.bufferView];
					const tinygltf::Buffer& buffer = gltfmodel.buffers[bufferView.buffer];
					const float* normalsgltf = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

					for (size_t i = 0; i < accessor.count; i++) {
						normals.push_back(glm::vec3(normalsgltf[i * 3 + 0], normalsgltf[i * 3 + 1], normalsgltf[i * 3 + 2]));
					}
				}
			}
			
		}

		Model* model = new Model(vertices, colors, indices, normals);

		return model;
	}

	Model* Model::createCube() {
		vector<glm::vec3> vertices = {
			{-1.0f,-1.0f,-1.0f}, // triangle 1 : begin
			{-1.0f,-1.0f, 1.0f},
			{-1.0f, 1.0f, 1.0f}, // triangle 1 : end
			{1.0f, 1.0f,-1.0f},
			{-1.0f,-1.0f,-1.0f},
			{-1.0f, 1.0f,-1.0f},
			{1.0f,-1.0f, 1.0f},
			{-1.0f,-1.0f,-1.0f},
			{1.0f,-1.0f,-1.0f},
			{1.0f, 1.0f,-1.0f},
			{1.0f,-1.0f,-1.0f},
			{-1.0f,-1.0f,-1.0f},
			{-1.0f,-1.0f,-1.0f},
			{-1.0f, 1.0f, 1.0f},
			{-1.0f, 1.0f,-1.0f},
			{1.0f,-1.0f, 1.0f},
			{-1.0f,-1.0f, 1.0f},
			{-1.0f,-1.0f,-1.0f},
			{-1.0f, 1.0f, 1.0f},
			{-1.0f,-1.0f, 1.0f},
			{1.0f,-1.0f, 1.0f},
			{1.0f, 1.0f, 1.0f},
			{1.0f,-1.0f,-1.0f},
			{1.0f, 1.0f,-1.0f},
			{1.0f,-1.0f,-1.0f},
			{1.0f, 1.0f, 1.0f},
			{1.0f,-1.0f, 1.0f},
			{1.0f, 1.0f, 1.0f},
			{1.0f, 1.0f,-1.0f},
			{-1.0f, 1.0f,-1.0f},
			{1.0f, 1.0f, 1.0f},
			{-1.0f, 1.0f,-1.0f},
			{-1.0f, 1.0f, 1.0f},
			{1.0f, 1.0f, 1.0f},
			{-1.0f, 1.0f, 1.0f},
			{1.0f,-1.0f, 1.0f}
		};

		vector<glm::vec3> rgb = {
			{0.583f,  0.771f,  0.014f},
			{0.609f, 0.115f, 0.436f},
			{0.327f,  0.483f,  0.844f},
			{0.822f,  0.569f,  0.201f},
			{0.435f,  0.602f,  0.223f},
			{0.310f,  0.747f,  0.185f},
			{0.597f,  0.770f,  0.761f},
			{0.559f,  0.436f,  0.730f},
			{0.359f,  0.583f,  0.152f},
			{0.483f,  0.596f,  0.789f},
			{0.559f,  0.861f,  0.639f},
			{0.195f,  0.548f,  0.859f},
			{0.014f,  0.184f,  0.576f},
			{0.771f,  0.328f,  0.970f},
			{0.406f,  0.615f,  0.116f},
			{0.676f,  0.977f,  0.133f},
			{0.971f,  0.572f,  0.833f},
			{0.140f,  0.616f,  0.489f},
			{0.997f,  0.513f,  0.064f},
			{0.945f,  0.719f,  0.592f},
			{0.543f,  0.021f,  0.978f},
			{0.279f,  0.317f,  0.505f},
			{0.167f,  0.620f,  0.077f},
			{0.347f,  0.857f,  0.137f},
			{0.055f,  0.953f,  0.042f},
			{0.714f,  0.505f,  0.345f},
			{0.783f,  0.290f,  0.734f},
			{0.722f,  0.645f,  0.174f},
			{0.302f,  0.455f,  0.848f},
			{0.225f,  0.587f,  0.040f},
			{0.517f,  0.713f,  0.338f},
			{0.053f,  0.959f,  0.120f},
			{0.393f,  0.621f,  0.362f},
			{0.673f,  0.211f,  0.457f},
			{0.820f,  0.883f,  0.371f},
			{0.982f,  0.099f,  0.879f}
		};

		Model* model = new Model(vertices, rgb, {}, {});

		return model;

	}
}