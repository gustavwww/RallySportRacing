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

	Model::Model(vector<glm::vec3> vertices, vector<glm::vec3> colors, vector<unsigned int> indices) {

		this->vertices = vertices;
		this->colors = colors;
		this->indices = indices;

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

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		// Pointer to which attribute to fill in vertex shader. (location=0)
		// Params: Attribute ID, size (x,y,z), type, normalized?, stride, array buffer offset.
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		
	}

	Model::~Model() {

		//glDeleteBuffers(1, &vertexBuffer);
		//glDeleteBuffers(1, &colorBuffer);
		//glDeleteVertexArrays(1, &vertexArrayID);
	}

	//Code for importing gltf without tinygltf library (Not currently used)

	/*std::vector<unsigned char> Model::getData() {
		std::string bytesText;
		std::string uri = JSON["buffers"][0]["uri"];
	}

	std::vector<float> Model::getFloats(json accessor) {
		std::vector<float> floatVec;

		unsigned int buffViewInd = accessor.value("bufferView", 1);
		unsigned int count = accessor["count"];
		unsigned int accByteOffset = accessor.value("byteOffset", 0);
		std::string type = accessor["type"];

		json bufferView = JSON["bufferViews"][buffViewInd];
		unsigned int byteOffset = bufferView["byteOffset"];

		unsigned int numPerVert;
		if (type == "SCALAR") numPerVert = 1;
		else if (type == "VEC2") numPerVert = 2;
		else if (type == "VEC3") numPerVert = 3;
		else if (type == "VEC4") numPerVert = 4;
		else throw std::invalid_argument("Type is invalid");

		unsigned int beginningOfData = byteOffset + accByteOffset;
		unsigned int lengthOfData = count * 4 * numPerVert;
		for (unsigned int i = beginningOfData; i < beginningOfData + lengthOfData; i) {
			unsigned char bytes[] = { data[i++], data[i++], data[i++], data[i++] };
			float value;
			std::memcpy(&value, bytes, sizeof(float));
			floatVec.push_back(value);
		}

		return floatVec;
	}

	std::vector<GLuint> Model::getIndices(json accessor) {
		std::vector<GLuint> indices;

		unsigned int buffViewInd = accessor.value("bufferView", 0);
		unsigned int count = accessor["count"];
		unsigned int accByteOffset = accessor.value("byteOffset", 0);

		json bufferView = JSON["bufferView"][buffViewInd];
		unsigned int byteOffset = bufferView["byteOffset"];

		unsigned int beginningOfData = byteOffset + accByteOffset;

		if (componentType == 5125) {
			for (unsigned int i = beginningOfData; i < beginningOfData + count * 4; i) {
				unsigned char bytes[] = { data[i++], data[i++], data[i++], data[i++] };
				unsigned int value;
				std::memcpy(&value, bytes, sizeof(unsigned int));
				indices.push_back((GLuint)value);
			}
		}
		else if (componentType == 5123) {
			for (unsigned int i = beginningOfData; i < beginningOfData + count * 2; i) {
				unsigned char bytes[] = { data[i++], data[i++] };
				unsigned short value;
				std::memcpy(&value, bytes, sizeof(unsigned short));
				indices.push_back((GLuint)value);
			}
		}
		else if (componentType == 5122) {
			for (unsigned int i = beginningOfData; i < beginningOfData + count * 2; i) {
				unsigned char bytes[] = { data[i++], data[i++] };
				short value;
				std::memcpy(&value, bytes, sizeof(short));
				indices.push_back((GLuint)value);
			}
		}

		return indices;
	}

	std::vector<Vertex> Model::assembleVertices(std::vector<glm::vec3> positions, std::vector<glm::vec2> texUVs, std::vector<glm::vec3> normals) {
		std::vector<Vertex> vertices;
		for (int i = 0; i < positions.size(); i++) {
			vertices.push_back(
				Vertex{
					positions[i],
					texUVs[i],
					normals[i]
				}
			);
		}
	}

	std::vector<glm::vec2> Model::groupFloatsVec2(std::vector<float> floatVec) {
		std::vector<glm::vec2> vectors;
		for (int i = 0; i < floatVec.size(); i) {
			vectors.push_back(glm::vec2(floatVec[i++], floatVec[i++]));
		}
		return vectors;
	}

	std::vector<glm::vec3> Model::groupFloatsVec3(std::vector<float> floatVec) {
		std::vector<glm::vec3> vectors;
		for (int i = 0; i < floatVec.size(); i) {
			vectors.push_back(glm::vec3(floatVec[i++], floatVec[i++], floatVec[i++]));
		}
		return vectors;
	}

	std::vector<glm::vec4> Model::groupFloatsVec4(std::vector<float> floatVec) {
		std::vector<glm::vec4> vectors;
		for (int i = 0; i < floatVec.size(); i) {
			vectors.push_back(glm::vec4(floatVec[i++], floatVec[i++], floatVec[i++], floatVec[i++]));
		}
		return vectors;
	}*/

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
		//glDisableVertexAttribArray(1);
	}

	Model* Model::createModel(const char* file) {

		tinygltf::Model gltfmodel;
		TinyGLTF loader;
		std::string err;
		std::string warn;
		vector<glm::vec3> vertices;
		vector<glm::vec3> colors;
		vector<unsigned int> indices;
		
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
						//if(i > accessor.count/2)
						colors.push_back(glm::vec3(0.f, 1.f, 0.f));
						//else
							//colors.push_back(glm::vec3(1.f, 0.f, 0.f));
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
			}
			
		}

		Model* model = new Model(vertices, colors, indices);

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

		Model* model = new Model(vertices, rgb, {});

		return model;

	}
}