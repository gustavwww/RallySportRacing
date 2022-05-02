#ifndef HDRFILEGENERATOR
#define HDRFILEGENERATOR

#include <GL/glew.h>
#include <string>
#include <iostream>
#include <stb_image.h>
#include <vector>
#include <glm/glm.hpp>

using namespace std;

namespace Utils {

	struct HDRImage
	{
		int width, height, components;
		float* data = nullptr;
		
		HDRImage(const std::string& filename)
		{
			stbi_set_flip_vertically_on_load(false);
			data = stbi_loadf(filename.c_str(), &width, &height, &components, 3);
			stbi_set_flip_vertically_on_load(true);
			if (data == nullptr)
			{
				std::cout << "Failed to load image: " << filename << ".\n";
				exit(1);
			}
		};
		
		~HDRImage()
		{
			stbi_image_free(data);
		};
	};

	class HdrFileGenerator {

	public:
		static void createIrradianceHDR(GLuint programID, string filePath);
		static void createReflectionHDRs(string filePath);

		static unsigned int loadHDRTexture(const std::string& filename);
		static unsigned int loadHdrMipmapTexture(const std::vector<std::string>& filenames);
		static void drawScreenQuad();

	private:
		static void setUpFrameBuffer(int resolution);
		static void setUpFrameBuffers();
		static void setUpTextureObject();
		
	};

}


#endif