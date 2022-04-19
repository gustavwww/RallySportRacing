#ifndef SDLWINDOWHANDLER
#define SDLWINDOWHANDLER

#include "Model.h"
//Remove this
#include "ParticleSystem.h"
#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <string>
#include <unordered_set>

using namespace std;

namespace Rendering {

	class SDLWindowHandler {
	public:
		SDLWindowHandler(int width, int height);
		void initSDLWindow(string caption);
		GLuint loadShader(const string& vertexShader, const string& fragmentShader);
		void beginRenderingLoop(void (*preRender)() = nullptr, void (*onQuit)() = nullptr);
		
		unsigned int loadTexture(const char* textureFilePath);
		unsigned int loadCubeMap(std::string folderDir);
		void renderBackground(GLint programID, unsigned int textureID, glm::mat4 view, glm::mat4 proj);
		void setupBackground();

		void addModel(Model* model);
		void removeModel(Model* model);

		void addParticleSystem(ParticleSystem* particleSystem);
		void removeParticlesSystem(ParticleSystem* particleSystem);

		SDL_Window* getSDLWindow();
		void Destroy();

		void setCamPosition(glm::vec3 camPos);
		void setCamDirection(glm::vec3 camDir);
		void setCamOrientation(glm::vec3 camOr);
		GLint getDebugID();



	private:
		SDL_Window* window = nullptr;
		SDL_GLContext context = NULL;
		int width;
		int height;
		unordered_set<Model*> models;
		unordered_set<ParticleSystem*> particleSystems;

		GLint debugID;
		glm::vec3 camPosition = glm::vec3(0, 0, 20);
		glm::vec3 camDirection = glm::vec3(0, 0, 0);
		glm::vec3 camOrientation = glm::vec3(0, 1, 0);
	};

}
#endif

