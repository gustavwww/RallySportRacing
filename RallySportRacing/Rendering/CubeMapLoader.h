#ifndef CUBEMAPLOADER
#define CUBEMAPLOADER
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>

using namespace std;

namespace Rendering {

	class CubemapLoader {

	public:
		static unsigned int convertHDRTextureToEnvironmentMap(GLint shaderProgram, string textureFilePath);
		static void renderBackground(GLint programID, unsigned int textureID, glm::mat4 view, glm::mat4 proj);

	private:

		//Functions
		static void setupFrameBuffer();
		static void loadHDRTexture(string filePath);
		static void setupCubemap();
		static void renderCube();
		static void setupSkyboxVAO();
	};
}
#endif