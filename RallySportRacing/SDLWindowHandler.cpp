#include "SDLWindowHandler.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

void SDLWindowHandler::initSDLWindow(string caption, int width, int height) {

	SDL_Init(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	SDL_Window* window = SDL_CreateWindow(caption.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL);

	SDL_GLContext context = SDL_GL_CreateContext(window);

	glewExperimental = GL_TRUE;

	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return;
	}

	// Define the viewport dimensions
	glViewport(0, 0, width, height);

	this->window = window;
	this->context = context;
}

GLuint SDLWindowHandler::loadShader(const string& vertexShader, const string& fragmentShader) {

	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read vertex and fragment shader
	ifstream vertexShaderStream(vertexShader, ios::in);
	if (!vertexShaderStream.is_open()) {
		printf("Could not open %s. Maybe in the wrong directory?\n", vertexShader.c_str());
		return 0;
	}
	
	stringstream vertexInput;
	vertexInput << vertexShaderStream.rdbuf();
	string vertexCode = vertexInput.str();

	ifstream fragmentShaderStream(fragmentShader, ios::in);
	if (!fragmentShaderStream.is_open()) {
		printf("Could not open %s. Maybe in the wrong directory?\n", fragmentShader.c_str());
		return 0;
	}

	stringstream fragmentInput;
	fragmentInput << fragmentShaderStream.rdbuf();
	string fragmentCode = fragmentInput.str();


	GLint result = GL_FALSE;
	int infoLogLength;

	// Compile vertex shaders
	const char* vertexSourcePointer = vertexCode.c_str();
	glShaderSource(vertexShaderID, 1, &vertexSourcePointer, NULL);
	glCompileShader(vertexShaderID);

	// Check if compiled properly
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0) {
		vector<char> error(infoLogLength + 1);
		glGetShaderInfoLog(vertexShaderID, infoLogLength, NULL, &error[0]);
		printf("%s\n", &error[0]);
	}

	// Compile fragment shader
	const char* fragmentSourcePointer = fragmentCode.c_str();
	glShaderSource(fragmentShaderID, 1, &fragmentSourcePointer, NULL);
	glCompileShader(fragmentShaderID);

	// Check if compiled properly
	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0) {
		vector<char> error(infoLogLength + 1);
		glGetShaderInfoLog(fragmentShaderID, infoLogLength, NULL, &error[0]);
		printf("%s\n", &error[0]);
	}

	// Link shader program
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	// Check if program linked properly
	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0) {
		vector<char> error(infoLogLength + 1);
		glGetProgramInfoLog(programID, infoLogLength, NULL, &error[0]);
		printf("%s\n", &error[0]);
	}

	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	return programID;
}

SDL_Window* SDLWindowHandler::getSDLWindow() {
	return window;
}

void SDLWindowHandler::Destroy() {
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}


