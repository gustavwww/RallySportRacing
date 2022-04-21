#ifndef CUBEMAPLOADER
#define CUBEMAPLOADER
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>

using namespace std;

namespace Rendering {

	static class CubemapLoader {
	
	public:
		static unsigned int convertHDRTextureToEnvironmentMap(GLint shaderProgram, string textureFilePath);
		
	private:
		
		//Framebuffer
		static unsigned int FBO;
		static unsigned int RBO;

		//Cube vertax array object
		static unsigned int cubeVAO;
		static unsigned int cubeVBO;

		//Texture related
		static unsigned int texture;
		static unsigned int cubemap;
		
		//Uniform values
		static glm::mat4 captureProjection;
		static glm::mat4 captureViews[];

		//Functions
		static void setupFrameBuffer();
		static void loadHDRTexture(string filePath);
		static unsigned int setupCubemap();
		static void setupCaptureProjectionAndViews();
		static void renderCube();
	};
}