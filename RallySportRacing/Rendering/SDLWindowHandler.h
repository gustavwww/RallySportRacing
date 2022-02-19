#ifndef SDLWINDOWHANDLER
#define SDLWINDOWHANDLER

#include "Model.h"
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
		void SDLWindowHandler::beginRenderingLoop(void (*preRender)() = nullptr);

		void addModel(Model* model);
		void removeModel(Model* model);

		SDL_Window* getSDLWindow();
		void Destroy();

	private:
		SDL_Window* window = nullptr;
		SDL_GLContext context = NULL;
		int width;
		int height;
		unordered_set<Model*> models;
	};

}
#endif

