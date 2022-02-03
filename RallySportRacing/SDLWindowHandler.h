#ifndef SDLWINDOWHANDLER
#define SDLWINDOWHANDLER

#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <string>

using namespace std;

	class SDLWindowHandler {
	public:
		void initSDLWindow(string caption, int width, int height);
		GLuint loadShader(const string& vertexShader, const string& fragmentShader);

		SDL_Window* getSDLWindow();
		void Destroy();

	private:
		SDL_Window* window = nullptr;
		SDL_GLContext context = NULL;

	};

#endif

