#include "SDLWindowHandler.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"


using namespace std;

namespace Rendering {

	//Global variables
	glm::vec3 lightColor = glm::vec3(1.f, 1.f, 1.f);
	glm::vec4 lightPos = glm::vec4(1.0f, 10.0f, 1.0f, 1.0f);

	void Rendering::SDLWindowHandler::setCamPosition(glm::vec3 camPos)
	{
		camPosition = camPos;
	}

	void Rendering::SDLWindowHandler::setCamDirection(glm::vec3 camDir)
	{
		camDirection = camDir;
	}


	void Rendering::SDLWindowHandler::setCamOrientation(glm::vec3 camOr)
	{
		camOrientation = camOr;
	}
	SDLWindowHandler::SDLWindowHandler(int width, int height) {
		this->width = width;
		this->height = height;
	}

	void SDLWindowHandler::initSDLWindow(string caption) {

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

		// Initialize ImGui; will allow us to edit variables in the application.
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplSDL2_InitForOpenGL( window, context );
		ImGui_ImplOpenGL3_Init();
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

	void SDLWindowHandler::beginRenderingLoop(void (*preRender)()) {

		GLint programID = loadShader("../RallySportRacing/Shaders/Shader.vert", "../RallySportRacing/Shaders/Shader.frag");

		// Params: field of view, perspective ratio, near clipping plane, far clipping plane.
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

		// Params: Cam pos in World Space, where to look at, head up (0,-1,0) = upside down.
		glm::mat4 view;

		// Send to GLSL shader
		GLuint matrixID = glGetUniformLocation(programID, "MVP");
		GLuint modelViewMatrixID = glGetUniformLocation(programID, "modelViewMatrix");
		GLuint normalMatrixID = glGetUniformLocation(programID, "normalMatrix");
		
		//GUI bool
		bool showDebugGUI = false;

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		
		SDL_Event windowEvent; 
		while (true) {
			// This needs to be the first thing checked for imgui to work well
			if (SDL_PollEvent(&windowEvent)) {
				ImGui_ImplSDL2_ProcessEvent(&windowEvent);
				if (windowEvent.type == SDL_QUIT) break;
			}
			//ImGui Setup
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame(window);
			ImGui::NewFrame();
			
			//Set slider to change in scene.
			ImGui::DragFloat3("light pos", &lightPos.x);
			ImGui::DragFloat3("light color", &lightColor.x);

			//Toggle DebugGUI with 'G'.
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_g) {
				showDebugGUI = !showDebugGUI;
			}
			
			view = glm::lookAt(camPosition, camDirection, camOrientation);
			glm::vec4 viewSpaceLightPos = view * lightPos;
			if (preRender) {
				(*preRender)();
			}

			// Clear the colorbuffer
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Choose shader program to use
			glUseProgram(programID);

			//Set uniforms for light source.
			glUniform3fv(glGetUniformLocation(programID, "viewSpaceLightPos"), 1, &viewSpaceLightPos[0]);
			glUniform3fv(glGetUniformLocation(programID, "lightColor"), 1, &lightColor[0]);

			for (Model* m : models) {
				m->render(matrixID, modelViewMatrixID, normalMatrixID, projection, view);
			}


			glBindFramebuffer( GL_FRAMEBUFFER, 0 );
			glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
			ImGui::Render();
			if (showDebugGUI) {
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			}
			SDL_GL_SwapWindow(window);
		}

	}

	void SDLWindowHandler::addModel(Model* model) {
		models.insert(model);
	}

	void SDLWindowHandler::removeModel(Model* model) {
		models.erase(model);
	}

	SDL_Window* SDLWindowHandler::getSDLWindow() {
		return window;
	}

	void SDLWindowHandler::Destroy() {
		ImGui_ImplSDL2_NewFrame( window );

		//Destroy imgui
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();

		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}
}