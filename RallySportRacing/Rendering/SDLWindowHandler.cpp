#include "SDLWindowHandler.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.inl>
#include <glm/ext/matrix_clip_space.hpp>
#include <stb_image.h>

#include <imgui.h>
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "Game/GameManager.h"
#include "Utils/HdrFileGenerator.h"
#include "Audio/audio.h"
#include "Networking/Networking.h"
#include "FrameBufferObject.h"
//#include "Fonts/eras-bold.otf"

using namespace std;

namespace Rendering {

	//Camera
	float nearPlane = 0.1f, farPlane = 2000.0f, fov = 45.0f;

	//Light
	glm::vec3 lightColor = glm::vec3(1.f, 1.f, 1.f);
	glm::vec4 lightPos = glm::vec4(-400.0f, 1000.0f, -1300.0f, 1.0f);
	float lightIntensity = 3.0f;
	float envMultiplier = 1.5f;

	//Shadow maps.
	FboInfo shadowMapFB;
	int shadowMapResolution = 8000;
	float polygonFactor = 3.5f, polygonUnits = 1.0f;

	//Audio
	Audio* sound;
	int volume = 50;

	//Menu
	bool startMenu = true;
	int menuButtonWidth = 300;
	int menuButtonHeight = 150;
	int settingsButtonSize = 96;
	int carColorCycleVariable = 0;
	std::string leaderboardContentList[3] = { "Gustav", "Oscar", "Daniel" };
	std::string leaderboardTimes[3] = { "1:28.32", "1:43.88", "1:55:72" };

	//Speed
	float trackerPos = 0.0f;
	float speed = 0.0f;

	//Timer
	int minutes = 0;
	float raceTime = 0;
	float countDownTime = 3.0f;
	bool raceFinished = false;

	float amplifier = 0.5f;
	double colorAmp = 0.5;

	glm::vec3 SDLWindowHandler::getLightPosition() {
		return glm::vec3(lightPos);
	}

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

		GLint programID = loadShader("../RallySportRacing/Shaders/Shader.vert", "../RallySportRacing/Shaders/Shader.frag");
		GLint particleProgramID = loadShader("../RallySportRacing/Shaders/Particle.vert", "../RallySportRacing/Shaders/Particle.frag");
		GLint skyboxProgramID = loadShader("../RallySportRacing/Shaders/Skybox.vert", "../RallySportRacing/Shaders/Skybox.frag");

		GLint hdrToCubemapID = loadShader("../RallySportRacing/Shaders/Cubemap.vert", "../RallySportRacing/Shaders/HdrToCubemap.frag");
		GLint text2DProgramID = loadShader("../RallySportRacing/Shaders/2DText.vert", "../RallySportRacing/Shaders/2DText.frag");

		GLint mapCreationID = loadShader("../RallySportRacing/Shaders/Environment.vert", "../RallySportRacing/Shaders/Environment.frag");
		GLint refMapCreationID = loadShader("../RallySportRacing/Shaders/Environment.vert", "../RallySportRacing/Shaders/ReflectivityMap.frag");
		GLint shadowMapID = loadShader("../RallySportRacing/Shaders/ShadowMap.vert", "../RallySportRacing/Shaders/ShadowMap.frag");

		debugID = loadShader("../RallySportRacing/Shaders/Hitbox.vert", "../RallySportRacing/Shaders/Hitbox.frag");
		int mainMenuTexture = loadTexture("../IMGS/main-menu.png");
		int startButtonTexture = loadTexture("../IMGS/start-button.png");
		int settingsButtonTexture = loadTexture("../IMGS/settings-button.png");
		int leaderboardButtonTexture = loadTexture("../IMGS/leaderboard-button.png");
		int settingsTexture = loadTexture("../IMGS/settingsbackground.png");
		int speedometerTexture = loadTexture("../IMGS/Speedometer-final.png");
		int speedTrackerTexture = loadTexture("../IMGS/Speed-tracker.png");
		int checkpoint = loadTexture("../IMGS/checkpoints.png");
		int leftButtonTexture1 = loadTexture("../IMGS/left-button.png");
		int rightButtonTexture1 = loadTexture("../IMGS/right-button.png");
		int leftButtonTexture2 = loadTexture("../IMGS/left-button.png");
		int rightButtonTexture2 = loadTexture("../IMGS/right-button.png");
		int blueCarTexture = loadTexture("../IMGS/blue-car.png");
		int greenCarTexture = loadTexture("../IMGS/green-car.png");
		int purpleCarTexture = loadTexture("../IMGS/pink-car.png");
		int greyCarTexture = loadTexture("../IMGS/grey-car.png");
		int orangeCarTexture = loadTexture("../IMGS/orange-car.png");
		int redCarTexture = loadTexture("../IMGS/red-car.png");
		int countOne = loadTexture("../IMGS/1.png");
		int countTwo = loadTexture("../IMGS/2.png");
		int countThree = loadTexture("../IMGS/3.png");
		int countGo = loadTexture("../IMGS/go.png");
		int checkpointZero = loadTexture("../IMGS/0.png");
		int checkpointOne = loadTexture("../IMGS/1.png");
		int checkpointTwo = loadTexture("../IMGS/2.png");
		int checkpointThree = loadTexture("../IMGS/3.png");
		int checkpointFour = loadTexture("../IMGS/4.png");
		int checkpointFive = loadTexture("../IMGS/5.png");
		int checkpointSix = loadTexture("../IMGS/6.png");
		int checkpointSeven = loadTexture("../IMGS/7.png");
		int leaderboardBackgroundTexture = loadTexture("../IMGS/leaderboard-background.png");
		int leaderboardBackgrundFillTexture = loadTexture("../IMGS/leaderboard-fill.png");
		int leaderboardTextTexture = loadTexture("../IMGS/leaderboard-text.png");
		int leaderboardBackButton = loadTexture("../IMGS/left-button.png");
		int settingsBackButton = loadTexture("../IMGS/left-button.png");
		int connectButton = loadTexture("../IMGS/connect-button.png");
		int finished = loadTexture("../IMGS/finished.png");

		// Params: field of view, perspective ratio, near clipping plane, far clipping plane.
		glm::mat4 projection = glm::perspective(glm::radians(fov), (float)width / (float)height, nearPlane, farPlane);
		glm::mat4 lightProjMatrix = glm::ortho(-50.f, 50.f, -50.f, 50.f, nearPlane, farPlane);

		// Params: Cam pos in World Space, where to look at, head up (0,-1,0) = upside down.
		glm::mat4 view;


		string backgroundFileName = "../Textures/Background/kloppenheim_06_2k";
		
		vector<string>reflectionLevels{
			backgroundFileName + "_0.hdr",
			backgroundFileName + "_1.hdr",
			backgroundFileName + "_2.hdr",
			backgroundFileName + "_3.hdr"
		};

		//Create irrdiance and reflection files.
		//Utils::HdrFileGenerator::createIrradianceHDR(mapCreationID, backgroundFileName + ".hdr");
		//Utils::HdrFileGenerator::createReflectionHDRs(refMapCreationID, backgroundFileName + ".hdr");

		//Load environment textures.
		unsigned int skybox = Utils::HdrFileGenerator::loadHDRTexture(backgroundFileName + ".hdr");
		unsigned int irradianceMap = Utils::HdrFileGenerator::loadHDRTexture("../Textures/Background/irradiance.hdr");
		unsigned int reflectionMap = Utils::HdrFileGenerator::loadHdrMipmapTexture(reflectionLevels);

		//Shadow map setup.
		shadowMapFB.resize(shadowMapResolution, shadowMapResolution);
		glBindTexture(GL_TEXTURE_2D, shadowMapFB.depthBuffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		//Bind textures.
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, skybox);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, irradianceMap);
		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_2D, reflectionMap);
		glActiveTexture(GL_TEXTURE9);
		glBindTexture(GL_TEXTURE_2D, shadowMapFB.depthBuffer);
		glActiveTexture(GL_TEXTURE0);

		//Environment uniforms.
		glUseProgram(skyboxProgramID);
		glUniform1fv(glGetUniformLocation(skyboxProgramID, "envMultiplier"), 1, &envMultiplier);

		//GUI bool
		bool showDebugGUI = false;
		bool mainMenu = false;
		bool settingsMenu = false;
		bool leaderboardMenu = false;
		bool speedometerActive = true;

		ImGuiIO& io = ImGui::GetIO();
		ImFont* font1 = io.Fonts->AddFontFromFileTTF("../Fonts\\Eras.ttf", 96);

		// Load character glyphs from font.
		loadCharacters();

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
				speedometerActive = !speedometerActive;
			}

			if (speedometerActive) {
				ImGui::SetNextWindowSize(ImVec2(750, 200), 0);
				ImGui::SetNextWindowPos(ImVec2(627, 920), 0);
				ImGui::Begin("Speedometer", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

				ImGui::Image((void*)(intptr_t)speedometerTexture, ImVec2(666, 146));

				ImGui::End();

				speed = Game::getVehicleSpeed();
				if (abs(speed) > 254) {
					trackerPos = 1222;
				}
				else {
					trackerPos = 648 + abs(speed * 2.3);
				}
				ImGui::SetNextWindowSize(ImVec2(750, 200), 0);
				ImGui::SetNextWindowPos(ImVec2(trackerPos, 976), 0);
				ImGui::Begin("Speed Tracker", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

				ImGui::Image((void*)(intptr_t)speedTrackerTexture, ImVec2(11, 80));

				ImGui::End();
			}


			if (mainMenu) {
				ImGui::SetNextWindowSize(ImVec2(width + 20, height + 20), 0);
				ImGui::SetNextWindowPos(ImVec2(-10, -10), 0);
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
				ImGui::Dummy(ImVec2(0, 450));
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
				if (ImGui::ImageButton((void*)(intptr_t)startButtonTexture, ImVec2(menuButtonWidth, menuButtonHeight))) { mainMenu = false; settingsMenu = false; leaderboardMenu = false; sound->playButtonPressSound();}

				ImGui::Dummy(ImVec2(0, 50));
				if (ImGui::ImageButton((void*)(intptr_t)settingsButtonTexture, ImVec2(menuButtonWidth, menuButtonHeight))) { mainMenu = false; settingsMenu = true; leaderboardMenu = false; sound->playButtonPressSound();}

				ImGui::Dummy(ImVec2(0, 50));
				if (ImGui::ImageButton((void*)(intptr_t)leaderboardButtonTexture, ImVec2(menuButtonWidth, menuButtonHeight))) { mainMenu = false; settingsMenu = false; leaderboardMenu = true; sound->playButtonPressSound();}

				ImGui::PopStyleColor(3);

				ImGui::End();
			}
			else if (settingsMenu) {
				ImGui::SetNextWindowSize(ImVec2(width + 20, height + 20), 0);
				ImGui::SetNextWindowPos(ImVec2(-10, -10), 0);
				ImGui::Begin("Main Menu", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

				ImGui::Image((void*)(intptr_t)settingsTexture, ImVec2(width, height));

				ImGui::End();

				ImGui::SetNextWindowSize(ImVec2(400, 100), 0);
				ImGui::SetNextWindowPos(ImVec2(822, 50), 0);
				ImGui::Begin("Text input", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

				ImGui::SetWindowFontScale(0.5);
				static char username[16] = "Username";
				ImGui::InputText("", username, IM_ARRAYSIZE(username));

				ImGui::End();

				ImGui::SetNextWindowSize(ImVec2(menuButtonWidth, menuButtonHeight), 0);
				ImGui::SetNextWindowPos(ImVec2(858, 120), 0);
				ImGui::Begin("Connect Button", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
				if (ImGui::ImageButton((void*)(intptr_t)connectButton, ImVec2(menuButtonWidth *0.6, menuButtonHeight*0.6))) { 
					sound->playButtonPressSound();
					//Networking::joinGame(0, username);
				}
				ImGui::PopStyleColor(3);

				ImGui::End();

				ImGui::SetNextWindowSize(ImVec2(width, 980), 0);
				ImGui::SetNextWindowPos(ImVec2(0, 200), 0);
				ImGui::Begin("Inner Main Menu", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

				ImGui::Dummy(ImVec2(0, 100));

				ImGui::Indent(width / 2 - menuButtonWidth / 2 - 280);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
				if (ImGui::ImageButton((void*)(intptr_t)leftButtonTexture1, ImVec2(settingsButtonSize, settingsButtonSize))) { if (volume >= 5) { sound->volumeDown(); volume = volume - 5; sound->playButtonPressSound();} }
				ImGui::SameLine();
				ImGui::Indent(150);
				ImGui::SetWindowFontScale(1);
				std::string volString = "Volume: " + std::to_string(volume) + "%%";
				char const* volChar = volString.c_str();
				ImGui::Text(volChar);
				ImGui::SameLine();
				ImGui::Indent(600);
				if (ImGui::ImageButton((void*)(intptr_t)rightButtonTexture1, ImVec2(settingsButtonSize, settingsButtonSize))) { if (volume <= 95) { sound->volumeUp(); volume = volume + 5; sound->playButtonPressSound();} }



				ImGui::Dummy(ImVec2(0, 200));

				ImGui::Indent(26 -284 - settingsButtonSize * 2 - menuButtonWidth);
				if (ImGui::ImageButton((void*)(intptr_t)leftButtonTexture2, ImVec2(settingsButtonSize, settingsButtonSize))) {
					sound->playButtonPressSound();
					carColorCycleVariable--;
					if (carColorCycleVariable < 0) { carColorCycleVariable = 5; }
					Game::setTextureIndex(carColorCycleVariable);
				}

				ImGui::SameLine();
				ImGui::Indent(settingsButtonSize + 69);
				if (carColorCycleVariable == 1) {
					ImGui::Image((void*)(intptr_t)redCarTexture, ImVec2(1051 * colorAmp, 829 * colorAmp));
				}
				else if (carColorCycleVariable == 2) {
					ImGui::Image((void*)(intptr_t)greenCarTexture, ImVec2(1051 * colorAmp, 829 * colorAmp));
				}
				else if (carColorCycleVariable == 3) {
					ImGui::Image((void*)(intptr_t)orangeCarTexture, ImVec2(1051 * colorAmp, 829 * colorAmp));
				}
				else if (carColorCycleVariable == 4) {
					ImGui::Image((void*)(intptr_t)purpleCarTexture, ImVec2(1051 * colorAmp, 829 * colorAmp));
				}
				else if (carColorCycleVariable == 5) {
					ImGui::Image((void*)(intptr_t)greyCarTexture, ImVec2(1051 * colorAmp, 829 * colorAmp));
				}
				else {
					ImGui::Image((void*)(intptr_t)blueCarTexture, ImVec2(1051 * colorAmp, 829 * colorAmp));
				}

				ImGui::SameLine();
				ImGui::Indent(settingsButtonSize + menuButtonWidth + 189);
				if (ImGui::ImageButton((void*)(intptr_t)rightButtonTexture2, ImVec2(settingsButtonSize, settingsButtonSize))) {
					sound->playButtonPressSound();
					carColorCycleVariable++;
					if (carColorCycleVariable > 5) { carColorCycleVariable = 0; }
					Game::setTextureIndex(carColorCycleVariable);
				}
				ImGui::PopStyleColor(3);

				ImGui::End();

				ImGui::SetNextWindowSize(ImVec2(menuButtonWidth + 20, menuButtonHeight + 20), 0);
				ImGui::SetNextWindowPos(ImVec2(50, 50), 0);
				ImGui::Begin("Settings Back-button", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
				if (ImGui::ImageButton((void*)(intptr_t)settingsBackButton, ImVec2(settingsButtonSize, settingsButtonSize))) { settingsMenu = false; sound->playButtonPressSound();}
				ImGui::PopStyleColor(3);

				ImGui::End();
			}
			else if (leaderboardMenu) {
				ImGui::SetNextWindowSize(ImVec2(width + 20, height + 20), 0);
				ImGui::SetNextWindowPos(ImVec2(-10, -10), 0);
				ImGui::Begin("Leaderboard Backfill", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

				ImGui::Image((void*)(intptr_t)leaderboardBackgrundFillTexture, ImVec2(width, height));

				ImGui::End();

				ImGui::SetNextWindowSize(ImVec2(width, height), 0);
				ImGui::SetNextWindowPos(ImVec2(0, 0), 0);
				ImGui::Begin("Leaderboard List", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

				ImGui::Dummy(ImVec2(0, 200));
				ImGui::Indent(660);

				ImGui::Image((void*)(intptr_t)leaderboardBackgroundTexture, ImVec2(600, 800));

				ImGui::End();


				ImGui::SetNextWindowSize(ImVec2(1300, 210), 0);
				ImGui::SetNextWindowPos(ImVec2(321, 10), 0);
				ImGui::Begin("Leaderboard Text", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

				ImGui::Image((void*)(intptr_t)leaderboardTextTexture, ImVec2(1278, 187));

				ImGui::End();

				ImGui::SetNextWindowSize(ImVec2(660, 800), 0);
				ImGui::SetNextWindowPos(ImVec2(660, 200), 0);
				ImGui::Begin("Leaderboard Content", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

				ImGui::Dummy(ImVec2(0, 50));
				ImGui::Indent(30);

				//vector<PlayerTime> = Networking::getTimes();

				ImGui::SetWindowFontScale(0.7);
				ImGui::Text("Gustav");
				ImGui::SameLine();
				ImGui::Indent(375);
				ImGui::Text("1:50.26");
				ImGui::Dummy(ImVec2(0, 50));
				ImGui::Indent(-375);
				ImGui::Text("Oscar");
				ImGui::SameLine();
				ImGui::Indent(375);
				ImGui::Text("1:57.83");

				ImGui::End();

				ImGui::SetNextWindowSize(ImVec2(menuButtonWidth + 20, menuButtonHeight + 20), 0);
				ImGui::SetNextWindowPos(ImVec2(50, 50), 0);
				ImGui::Begin("Leaderboard Back-button", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
				if (ImGui::ImageButton((void*)(intptr_t)leaderboardBackButton, ImVec2(settingsButtonSize, settingsButtonSize))) { leaderboardMenu = false; sound->playButtonPressSound();}
				ImGui::PopStyleColor(3);

				ImGui::End();
			}

			countDownTime = Game::getCountDownTime();
			if (countDownTime < 3.0f) {
				ImGui::SetNextWindowSize(ImVec2(300, 300), 0);
				ImGui::SetNextWindowPos(ImVec2(width / 2 - 100 - 10, height / 2 - 160), 0);
				ImGui::Begin("Count Down", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

				if (countDownTime > 2) {
					ImGui::Image((void*)(intptr_t)countThree, ImVec2(209, 266));
				}
				else if (countDownTime > 1) {
					ImGui::Image((void*)(intptr_t)countTwo, ImVec2(216, 261));
				}
				else if (countDownTime > 0) {
					ImGui::Indent(30);
					ImGui::Image((void*)(intptr_t)countOne, ImVec2(129, 256));
				}


				ImGui::End();
			}


			raceTime = Game::getRaceTime();


			if (raceTime > 0 && raceTime < 1) {
				ImGui::SetNextWindowSize(ImVec2(700, 300), 0);
				ImGui::SetNextWindowPos(ImVec2(width / 2 - 607 / 2, height / 2 - 300 / 2 - 30), 0);
				ImGui::Begin("Count Go", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

				ImGui::Image((void*)(intptr_t)countGo, ImVec2(607, 266));

				ImGui::End();

				raceFinished = false;
			}
			else if (raceTime > 1) {
				if (raceFinished == false) {
					ImGui::SetNextWindowSize(ImVec2(700, 300), 0);
					ImGui::SetNextWindowPos(ImVec2(0, 0), 0);
					ImGui::Begin("Race Timer", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
					ImGui::SetWindowFontScale(1.5);
					ImGui::PushFont(font1);
					if (raceTime >= 60) {
						while (raceTime > 60) {
							raceTime = raceTime - 60;
							minutes++;
						}
						std::string timerString = std::to_string(raceTime);
						if (raceTime > 10) {
							timerString = timerString.substr(0, 5);
						}
						else {
							timerString = timerString.substr(0, 4);
							timerString = "0" + timerString;
						}

						std::string minutesString = std::to_string(minutes);
						std::string finalString = minutesString + ":" + timerString;
						char const* timerChar = finalString.c_str();
						ImGui::Text(timerChar);
					}
					else {
						std::string timerString = std::to_string(raceTime);
						if (raceTime > 10) {
							timerString = timerString.substr(0, 5);
						}
						else {
							timerString = timerString.substr(0, 4);
							timerString = "0" + timerString;
						}
						char const* timerChar = timerString.c_str();
						ImGui::Text(timerChar);
					}
					ImGui::PopFont();
					ImGui::End();

					ImGui::SetNextWindowSize(ImVec2(700, 400), 0);
					ImGui::SetNextWindowPos(ImVec2(1550, 30), 0);
					ImGui::Begin("Checkpoints", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

					ImGui::Image((void*)(intptr_t)checkpoint, ImVec2(636 * amplifier, 350 * amplifier));

					ImGui::End();
				}

				minutes = 0;

				switch (Game::getCheckpointsReached()) {
				case 0:
					if (raceTime > 1 && raceTime < 100) {
						ImGui::SetNextWindowSize(ImVec2(700, 400), 0);
						ImGui::SetNextWindowPos(ImVec2(1550, 33), 0);
						ImGui::Begin("CheckpointZero", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

						ImGui::Image((void*)(intptr_t)checkpointZero, ImVec2(224 * amplifier, 266 * amplifier));

						ImGui::End();
					}
					break;
				case 1:
					ImGui::SetNextWindowSize(ImVec2(700, 400), 0);
					ImGui::SetNextWindowPos(ImVec2(1550, 33), 0);
					ImGui::Begin("CheckpointsOne", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

					ImGui::Image((void*)(intptr_t)checkpointOne, ImVec2(129 * amplifier, 256 * amplifier));

					ImGui::End();
					break;
				case 2:
					ImGui::SetNextWindowSize(ImVec2(700, 400), 0);
					ImGui::SetNextWindowPos(ImVec2(1550, 33), 0);
					ImGui::Begin("CheckpointsTwo", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

					ImGui::Image((void*)(intptr_t)checkpointTwo, ImVec2(216 * amplifier, 261 * amplifier));

					ImGui::End();
					break;
				case 3:
					ImGui::SetNextWindowSize(ImVec2(700, 400), 0);
					ImGui::SetNextWindowPos(ImVec2(1550, 33), 0);
					ImGui::Begin("CheckpointsThree", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

					ImGui::Image((void*)(intptr_t)checkpointThree, ImVec2(209 * amplifier, 266 * amplifier));

					ImGui::End();
					break;
				case 4:
					ImGui::SetNextWindowSize(ImVec2(700, 400), 0);
					ImGui::SetNextWindowPos(ImVec2(1550, 33), 0);
					ImGui::Begin("CheckpointsFour", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

					ImGui::Image((void*)(intptr_t)checkpointFour, ImVec2(224 * amplifier, 256 * amplifier));

					ImGui::End();
					break;
				case 5:
					ImGui::SetNextWindowSize(ImVec2(700, 470), 0);
					ImGui::SetNextWindowPos(ImVec2(1550, 33), 0);
					ImGui::Begin("CheckpointsFive", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

					ImGui::Image((void*)(intptr_t)checkpointFive, ImVec2(204 * amplifier, 277 * amplifier));

					ImGui::End();
					break;
				case 6:
					ImGui::SetNextWindowSize(ImVec2(700, 450), 0);
					ImGui::SetNextWindowPos(ImVec2(1550, 33), 0);
					ImGui::Begin("CheckpointsSix", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

					ImGui::Image((void*)(intptr_t)checkpointSix, ImVec2(221 * amplifier, 258 * amplifier));

					ImGui::End();
					break;
				case 7:
					ImGui::SetNextWindowSize(ImVec2(700, 450), 0);
					ImGui::SetNextWindowPos(ImVec2(1550, 33), 0);
					ImGui::Begin("CheckpointsSeven", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

					ImGui::Image((void*)(intptr_t)checkpointSeven, ImVec2(203 * amplifier, 256 * amplifier));

					ImGui::End();
					break;
				case 8:
					raceFinished = true;

					ImGui::SetNextWindowSize(ImVec2(1000, 450), 0);
					ImGui::SetNextWindowPos(ImVec2(712, 200), 0);
					ImGui::Begin("Race Finished", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

					ImGui::SetWindowFontScale(2);

					if (raceTime >= 60) {
						while (raceTime > 60) {
							raceTime = raceTime - 60;
							minutes++;
						}
						std::string timerString = std::to_string(raceTime);
						if (raceTime > 10) {
							timerString = timerString.substr(0, 5);
						}
						else {
							timerString = timerString.substr(0, 4);
							timerString = "0" + timerString;
						}

						std::string minutesString = std::to_string(minutes);
						std::string finalString = minutesString + ":" + timerString;
						char const* timerChar = finalString.c_str();
						ImGui::Text(timerChar);
					}
					else {
						std::string timerString = std::to_string(raceTime);
						if (raceTime > 10) {
							timerString = timerString.substr(0, 5);
						}
						else {
							timerString = timerString.substr(0, 4);
							timerString = "0" + timerString;
						}
						char const* timerChar = timerString.c_str();
						ImGui::Text(timerChar);
					}

					ImGui::End();

					ImGui::SetNextWindowSize(ImVec2(1410, 230), 0);
					ImGui::SetNextWindowPos(ImVec2(280, 380), 0);
					ImGui::Begin("Finished", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

					ImGui::Image((void*)(intptr_t)finished, ImVec2(1396, 201));

					ImGui::End();

					Game::turnOffEngine();

					break;
				case 9:
					raceTime = 0;
					break;
				}
			}


			if (showDebugGUI) {
				ImGui::SetNextWindowSize(ImVec2(300, 300), 0);
				ImGui::SetNextWindowPos(ImVec2(300, 300), 0);
				ImGui::Begin("Options", 0, 0);

				//Set slider to change in scene.
				ImGui::DragFloat3("light pos", &lightPos.x);
				ImGui::DragFloat3("light color", &lightColor.x);
				ImGui::DragFloat("light intensity", &lightIntensity);
				ImGui::DragFloat("polygon factor", &polygonFactor);
				ImGui::DragFloat("polygon units", &polygonUnits);
				ImGui::DragFloat("EnvMult", &envMultiplier);

				glUseProgram(shadowMapID);
				ImGui::Image((ImTextureID)shadowMapFB.colorTextureTarget, ImVec2(700, 700));
				
				ImGui::End();
			}

			view = glm::lookAt(camPosition, camDirection, camOrientation);
			glm::vec4 viewSpaceLightPos = view * lightPos;
			if (preRender) {
				(*preRender)();
			}

			//Shadow map creation.
			glm::mat4 lightViewMatrix = glm::lookAt(glm::vec3(lightPos) + camPosition, camPosition, glm::vec3(0.0f, 1.0f, 0.0f));

			glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFB.framebufferId);
			glViewport(0, 0, shadowMapFB.width, shadowMapFB.height);
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(polygonFactor, polygonUnits);
			glUseProgram(shadowMapID);
			for (Model* m : models) {
				m->render(lightProjMatrix, lightViewMatrix, shadowMapID);
			}
			glDisable(GL_POLYGON_OFFSET_FILL);

			//Set deafult framebuffer and viewport.
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, width, height);

			// Clear the colorbuffer
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//Draw background
			glUseProgram(skyboxProgramID);
			glUniform3fv(glGetUniformLocation(skyboxProgramID, "cameraPos"), 1, &camPosition[0]);
			glUniformMatrix4fv(glGetUniformLocation(skyboxProgramID, "invPV"), 1, GL_FALSE, &inverse(projection * view)[0][0]);
			Utils::HdrFileGenerator::drawScreenQuad();

			// Choose shader program to use
			glUseProgram(programID);

			//Set uniforms for light source.
			glm::mat4 lightMatrix = glm::translate(glm::vec3(0.5f)) * glm::scale(glm::vec3(0.5f)) * lightProjMatrix * lightViewMatrix * inverse(view);
			glUniform3fv(glGetUniformLocation(programID, "viewSpaceLightPos"), 1, &viewSpaceLightPos[0]);
			glUniform3fv(glGetUniformLocation(programID, "lightColor"), 1, &lightColor[0]);
			glUniform1fv(glGetUniformLocation(programID, "lightIntensity"), 1, &lightIntensity);
			glUniform1fv(glGetUniformLocation(programID, "envMultiplier"), 1, &envMultiplier);
			glUniformMatrix4fv(glGetUniformLocation(programID, "lightMatrix"), 1, GL_FALSE, &lightMatrix[0][0]);


			for (Model* m : models) {
				m->render(projection, view, programID);
			}

			for (ParticleSystem* p : particleSystems) {
				p->render(particleProgramID, projection, view, width, height);
			}

			for (Text2D* t : texts) {
				t->render(text2DProgramID, projection, view);
			}

			Game::drawDebug();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

	void SDLWindowHandler::addText(Text2D* text) {
		texts.insert(text);
	}

	void SDLWindowHandler::removeText(Text2D* text) {
		texts.erase(text);
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