#include "SDLWindowHandler.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

#include <imgui.h>
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "Game/GameManager.h"
#include "CubemapLoader.h"
#include "Audio/audio.h"

using namespace std;


namespace Rendering {

	//Light
	glm::vec3 lightColor = glm::vec3(1.f, 1.f, 1.f);
	glm::vec4 lightPos = glm::vec4(1.0f, 10.0f, 1.0f, 1.0f);
	
	//Audio
	Audio* sound;
	int volume = 50;

	//Menu
	bool startMenu = true;
	int menuButtonWidth = 300;
	int menuButtonHeight = 150;
	int settingsButtonSize = 96;
	int carColorCycleVariable = 0;

	//Speed
	float speed = 0.0f;

	GLint Rendering::SDLWindowHandler::getDebugID()
	{
		return this->debugID;
	}

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

	unsigned int SDLWindowHandler::loadTexture(const char* textureFilePath) {

		ifstream textureStream(textureFilePath, ios::in);
		if (!textureStream.is_open()) {
			printf("Could not open %s. Maybe in the wrong directory?\n", textureFilePath);
			return 0;
		}

		int width, height, nrChannels;
		unsigned char* image = stbi_load(textureFilePath, &width, &height, &nrChannels, STBI_rgb_alpha);
		unsigned int textureID;

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		free(image);

		//Set wrapping type.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//Mipmap and filtering.
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);

		return textureID;
	}

	void SDLWindowHandler::beginRenderingLoop(void (*preRender)(), void (*onQuit)()) {

		sound = Audio::Instance();

		GLint programID = loadShader("../RallySportRacing/Shaders/Shader.vert", "../RallySportRacing/Shaders/Shader.frag");
		GLint particleProgramID = loadShader("../RallySportRacing/Shaders/Particle.vert", "../RallySportRacing/Shaders/Particle.frag");
		GLint skyboxProgramID = loadShader("../RallySportRacing/Shaders/Skybox.vert", "../RallySportRacing/Shaders/Skybox.frag");
		GLint hdrToCubemapID = loadShader("../RallySportRacing/Shaders/Cubemap.vert", "../RallySportRacing/Shaders/HdrToCubemap.frag");

		debugID = loadShader("../RallySportRacing/Shaders/Hitbox.vert", "../RallySportRacing/Shaders/Hitbox.frag");
		int mainMenuTexture = loadTexture("../IMGS/main-menu.png");
		int startButtonTexture = loadTexture("../IMGS/start-button.png");
		int settingsButtonTexture = loadTexture("../IMGS/settings-button.png");
		int settingsTexture = loadTexture("../IMGS/settingsbackground.png");
		int speedometerTexture = loadTexture("../IMGS/Speedometer-final.png");
		int speedTrackerTexture = loadTexture("../IMGS/Speed-tracker.png");
		int firstPosition = loadTexture("../IMGS/1st.png");
		int leftButtonTexture1 = loadTexture("../IMGS/left-button.png");
		int rightButtonTexture1 = loadTexture("../IMGS/right-button.png");
		int leftButtonTexture2 = loadTexture("../IMGS/left-button.png");
		int rightButtonTexture2 = loadTexture("../IMGS/right-button.png");
		int blueCarTexture = loadTexture("../IMGS/blue-car.png");
		int greenCarTexture = loadTexture("../IMGS/green-car.png");
		int pinkCarTexture = loadTexture("../IMGS/pink-car.png");

		// Params: field of view, perspective ratio, near clipping plane, far clipping plane.
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 5000.0f);

		// Params: Cam pos in World Space, where to look at, head up (0,-1,0) = upside down.
		glm::mat4 view;

		//Load skybox.
		unsigned int skybox = CubemapLoader::convertHDRTextureToEnvironmentMap(hdrToCubemapID, "../Textures/Background/cape_hill_2k.hdr");
		
		//GUI bool
		bool showDebugGUI = false;
		bool mainMenu = false;
		bool settingsMenu = false;

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		
		SDL_Event windowEvent; 
		while (true) {
			// This needs to be the first thing checked for imgui to work well
			if (SDL_PollEvent(&windowEvent)) {
				ImGui_ImplSDL2_ProcessEvent(&windowEvent);
				if (windowEvent.type == SDL_QUIT) {
					if (onQuit) {
						(*onQuit)();
					}
					break;
				}
			}
			//ImGui Setup
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame(window);
			ImGui::NewFrame();
			

			//Toggle DebugGUI with 'G'.
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_g) {
				showDebugGUI = !showDebugGUI;
			}

			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_m) {
				mainMenu = !mainMenu;
			}

			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_6) {
				//speed = vehicle->getSpeed();
			}
			
			ImGui::SetNextWindowSize(ImVec2(750, 200), 0);
			ImGui::SetNextWindowPos(ImVec2(627, 920), 0);
			ImGui::Begin("Speedometer", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

			ImGui::Image((void*)(intptr_t)speedometerTexture, ImVec2(666, 146));

			ImGui::End();


			ImGui::SetNextWindowSize(ImVec2(750, 200), 0);
			ImGui::SetNextWindowPos(ImVec2(680, 966), 0);
			ImGui::Begin("Speed Tracker", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

			ImGui::Image((void*)(intptr_t)speedTrackerTexture, ImVec2(11, 100));

			ImGui::End();


			ImGui::SetNextWindowSize(ImVec2(300, 300), 0);
			ImGui::SetNextWindowPos(ImVec2(1600, 0), 0);
			ImGui::Begin("Position tracker", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

			ImGui::Image((void*)(intptr_t)firstPosition, ImVec2(256, 256));

			ImGui::End();


			if (mainMenu) {
				ImGui::SetNextWindowSize(ImVec2(width, height), 0);
				ImGui::SetNextWindowPos(ImVec2(0, 0), 0);
				ImGui::Begin("Main Menu", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

				//ImGui::BeginChildFrame('h', ImVec2(width, height));
				ImGui::Image((void*)(intptr_t)mainMenuTexture, ImVec2(width, height));
				//ImGui::EndChildFrame();

				ImGui::End();

				ImGui::SetNextWindowSize(ImVec2(width, height), 0);
				ImGui::SetNextWindowPos(ImVec2(0, 0), 0);
				ImGui::Begin("Inner Main Menu", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

				ImGui::Indent(width / 2 - menuButtonWidth / 2);
				//ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 600));
				if (ImGui::ImageButton((void*)(intptr_t)startButtonTexture, ImVec2(menuButtonWidth, menuButtonHeight))) { mainMenu = false; settingsMenu = false; }


				ImGui::Dummy(ImVec2(0, 100));
				if (ImGui::ImageButton((void*)(intptr_t)settingsButtonTexture, ImVec2(menuButtonWidth, menuButtonHeight))) { mainMenu = false; settingsMenu = true; }

				ImGui::End();
			}
			else if (settingsMenu) {
				ImGui::SetNextWindowSize(ImVec2(width, height), 0);
				ImGui::SetNextWindowPos(ImVec2(0, 0), 0);
				ImGui::Begin("Main Menu", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

				//ImGui::BeginChildFrame('h', ImVec2(width, height));
				ImGui::Image((void*)(intptr_t)settingsTexture, ImVec2(width, height));
				//ImGui::EndChildFrame();

				ImGui::End();

				ImGui::SetNextWindowSize(ImVec2(width, height), 0);
				ImGui::SetNextWindowPos(ImVec2(0, 0), 0);
				ImGui::Begin("Inner Main Menu", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

				ImGui::Dummy(ImVec2(0, 250));

				ImGui::Indent(width / 2 - menuButtonWidth / 2 - 200);
				if (ImGui::ImageButton((void*)(intptr_t)leftButtonTexture1, ImVec2(settingsButtonSize, settingsButtonSize))) { if (volume >= 5) { sound->volumeDown(); volume = volume - 5; } }
				ImGui::SameLine();
				ImGui::Indent(300);
				std::string volString = std::to_string(volume) + "%%";
				char const* volChar = volString.c_str();
				ImGui::Text(volChar);
				ImGui::SameLine();
				ImGui::Indent(300);
				if (ImGui::ImageButton((void*)(intptr_t)rightButtonTexture1, ImVec2(settingsButtonSize, settingsButtonSize))) { if (volume <= 95) { sound->volumeUp(); volume = volume + 5; } }

				

				ImGui::Dummy(ImVec2(0, 300));

				ImGui::Indent(-100 - settingsButtonSize*2 - menuButtonWidth);
				if (ImGui::ImageButton((void*)(intptr_t)leftButtonTexture2, ImVec2(settingsButtonSize, settingsButtonSize))) {
					carColorCycleVariable--;
					if (carColorCycleVariable < 0) { carColorCycleVariable = 2; }
				}

				ImGui::SameLine();
				ImGui::Indent(settingsButtonSize + 100);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
				if (carColorCycleVariable == 1) {
					if (ImGui::ImageButton((void*)(intptr_t)pinkCarTexture, ImVec2(menuButtonWidth, menuButtonHeight))) { settingsMenu = false; }
				}
				else if (carColorCycleVariable == 2) {
					if (ImGui::ImageButton((void*)(intptr_t)greenCarTexture, ImVec2(menuButtonWidth, menuButtonHeight))) { settingsMenu = false; }
				}
				else {
					if (ImGui::ImageButton((void*)(intptr_t)blueCarTexture, ImVec2(menuButtonWidth, menuButtonHeight))) { settingsMenu = false; }
				}
				
				ImGui::PopStyleColor(3);
				ImGui::SameLine();
				ImGui::Indent(settingsButtonSize + menuButtonWidth);
				if (ImGui::ImageButton((void*)(intptr_t)rightButtonTexture2, ImVec2(settingsButtonSize, settingsButtonSize))) {
					carColorCycleVariable++;
					if (carColorCycleVariable > 2) { carColorCycleVariable = 0; }
				}

				ImGui::End();
			}
			


			if (showDebugGUI) {
				ImGui::SetNextWindowSize(ImVec2(300, 300), 0);
				ImGui::SetNextWindowPos(ImVec2(300, 300), 0);
				ImGui::Begin("Options", 0, 0);

				//Set slider to change in scene.
				ImGui::DragFloat3("light pos", &lightPos.x);
				ImGui::DragFloat3("light color", &lightColor.x);

				ImGui::End();
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
				m->render(projection, view, programID);
			}

			for (ParticleSystem* p : particleSystems) {
				p->render(particleProgramID, projection, view, width, height);
			}

			//ToDo draw background here.
			CubemapLoader::renderBackground(skyboxProgramID, skybox, view, projection);

			Game::drawDebug();

			glBindFramebuffer( GL_FRAMEBUFFER, 0 );
			glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
			ImGui::Render();
			
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			
			SDL_GL_SwapWindow(window);
		}

	}

	void SDLWindowHandler::addModel(Model* model) {
		models.insert(model);
	}

	void SDLWindowHandler::removeModel(Model* model) {
		models.erase(model);
	}

	void SDLWindowHandler::addParticleSystem(ParticleSystem* particleSystem) {
		particleSystems.insert(particleSystem);
	}
	void SDLWindowHandler::removeParticlesSystem(ParticleSystem* particleSystem) {
		particleSystems.erase(particleSystem);
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