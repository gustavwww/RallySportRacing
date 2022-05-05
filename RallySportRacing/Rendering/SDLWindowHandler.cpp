#include "SDLWindowHandler.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <stb_image.h>

#include <imgui.h>
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "Game/GameManager.h"
#include "Utils/HdrFileGenerator.h"

using namespace std;

namespace Rendering {

	//Light
	glm::vec3 lightColor = glm::vec3(1.f, 1.f, 1.f);
	glm::vec4 lightPos = glm::vec4(1.0f, 40.0f, 1.0f, 1.0f);
	float envMultiplier = 1.5f;
	
	//Shadow maps.
	unsigned int shadowMap;
	unsigned int shadowMapFBO;
	int shadowMapResolution = 2048;
	float nearPlane = 1.0f, farPlane = 75.0f;
	

	//TESTING VARIABLE
	unsigned int quadVAO = 0;
	unsigned int quadVBO;

	//Audio
	int volume = 50;

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
		GLint mapCreationID = loadShader("../RallySportRacing/Shaders/Environment.vert", "../RallySportRacing/Shaders/Environment.frag");
		GLint shadowMapID = loadShader("../RallySportRacing/Shaders/ShadowMap.vert", "../RallySportRacing/Shaders/ShadowMap.frag");
		GLint shadowDebugID = loadShader("../RallySportRacing/Shaders/ShadowDebug.vert", "../RallySportRacing/Shaders/ShadowDebug.frag");

		debugID = loadShader("../RallySportRacing/Shaders/Hitbox.vert", "../RallySportRacing/Shaders/Hitbox.frag");

		// Params: field of view, perspective ratio, near clipping plane, far clipping plane.
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 5000.0f);

		// Params: Cam pos in World Space, where to look at, head up (0,-1,0) = upside down.
		glm::mat4 view;


		string backgroundFileName = "../Textures/Background/001";
		vector<string>reflectionLevels{
			backgroundFileName + "_dl_0.hdr",
			backgroundFileName + "_dl_1.hdr",
			backgroundFileName + "_dl_2.hdr",
			backgroundFileName + "_dl_3.hdr",
			backgroundFileName + "_dl_4.hdr",
			backgroundFileName + "_dl_5.hdr",
			backgroundFileName + "_dl_6.hdr",
			backgroundFileName + "_dl_7.hdr"
		};

		//Create irrdiance and reflection files.
		//Utils::HdrFileGenerator::createIrradianceHDR(mapCreationID, backgroundFileName + ".hdr");
		//Utils::HdrFileGenerator::createReflectionHDRs(mapCreationID, backgroundFileName + ".hdr");

		//Load environment textures.
		unsigned int skybox = Utils::HdrFileGenerator::loadHDRTexture(backgroundFileName + ".hdr");
		unsigned int irradianceMap = Utils::HdrFileGenerator::loadHDRTexture("../Textures/Background/irradiance.hdr");
		unsigned int reflectionMap = Utils::HdrFileGenerator::loadHdrMipmapTexture(reflectionLevels);

		//Bind textures.
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, skybox);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, irradianceMap);
		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_2D, reflectionMap);

		//Environment uniforms.
		glUseProgram(skyboxProgramID);
		glUniform1fv(glGetUniformLocation(skyboxProgramID, "envMultiplier"), 1, &envMultiplier);

		//GUI bool
		bool showDebugGUI = false;

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

			//Set slider to change in scene.
			ImGui::DragFloat3("light pos", &lightPos.x);
			ImGui::DragFloat3("light color", &lightColor.x);

			//Toggle DebugGUI with 'G'.
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_g) {

				showDebugGUI = !showDebugGUI;
			}

			//Volume control menu
			if (ImGui::ArrowButton("volDownButton", 0)) { if (volume >= 5) { volume = volume - 5; cout << "Left button!\n"; } }
			ImGui::SameLine(50);
			ImGui::Text("VOLUME: ");
			ImGui::SameLine(150);
			std::string volString = std::to_string(volume) + "%%";
			char const* volChar = volString.c_str();
			ImGui::Text(volChar);
			ImGui::SameLine(200);
			if (ImGui::ArrowButton("volUpButton", 1)) { if (volume <= 95) { volume = volume + 5; cout << "Right button!\n"; } }

			view = glm::lookAt(camPosition, camDirection, camOrientation);
			glm::vec4 viewSpaceLightPos = view * lightPos;
			if (preRender) {
				(*preRender)();
			}

			setUpShadowMapFrameBuffer();

			
			//ToDo mess around with these numbers until the shadows look good.
			ImGui::DragFloat("shadowFarPlane", &farPlane);
			ImGui::DragFloat("shadowNearPlane", &nearPlane);
			glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
			glm::mat4 lightView = glm::lookAt(glm::vec3(lightPos), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
			glm::mat4 lightSpaceMatrix = lightProjection * lightView;

			glUseProgram(shadowMapID);
			glUniformMatrix4fv(glGetUniformLocation(shadowMapID, "lightProjection"), 1, GL_FALSE, &lightSpaceMatrix[0][0]);

			//glEnable(GL_DEPTH_TEST);
			glViewport(0, 0, shadowMapResolution, shadowMapResolution);
			glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);

			

			//ToDo render all models here.
			for (Model* m : models) {
				m->render(projection, view, shadowMapID);
			}
			//Set deafult framebuffer and viewport.
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, width, height);

			// Clear the colorbuffer
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glActiveTexture(GL_TEXTURE9);
			glBindTexture(GL_TEXTURE_2D, shadowMap);

			//Draw background
			glUseProgram(skyboxProgramID);
			glUniform3fv(glGetUniformLocation(skyboxProgramID, "cameraPos"), 1, &camPosition[0]);
			glUniformMatrix4fv(glGetUniformLocation(skyboxProgramID, "invPV"), 1, GL_FALSE, &inverse(projection * view)[0][0]);
			Utils::HdrFileGenerator::drawScreenQuad();

			// Choose shader program to use
			glUseProgram(programID);

			//Set uniforms for light source.
			glUniform3fv(glGetUniformLocation(programID, "viewSpaceLightPos"), 1, &viewSpaceLightPos[0]);
			glUniform3fv(glGetUniformLocation(programID, "lightColor"), 1, &lightColor[0]);
			glUniformMatrix4fv(glGetUniformLocation(programID, "lightSpaceMatrix"), 1, GL_FALSE, &lightSpaceMatrix[0][0]);


			for (Model* m : models) {
				m->render(projection, view, programID);
			}

			for (ParticleSystem* p : particleSystems) {
				p->render(particleProgramID, projection, view, width, height);
			}

			//Render shadow map for testing.
			//debugRenderShadowMap(shadowDebugID);

			//Game::drawDebug();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
			ImGui::Render();
			if (showDebugGUI) {
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			}
			SDL_GL_SwapWindow(window);
		}

	}

	void SDLWindowHandler::setUpShadowMapFrameBuffer() {
		if (shadowMapFBO != 0) {
			return;
		}

		//Generate depthBuffer.
		glGenFramebuffers(1, &shadowMapFBO);

		//Generate depthMapTexture.
		glGenTextures(1, &shadowMap);
		glBindTexture(GL_TEXTURE_2D, shadowMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapResolution, shadowMapResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		//Setup for framebuffer.
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);

		//Prevent from reading or writing to the color buffer.
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		//Set default framebuffer.
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void SDLWindowHandler::debugRenderShadowMap(GLuint programID) {
		glViewport(0, 0, shadowMapResolution, shadowMapResolution);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(programID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowMap);
		debugRenderQuad();
		//Reset viewport.
		glViewport(0, 0, width, height);
	}

	void SDLWindowHandler::debugRenderQuad() {
		if (quadVAO == 0)
		{
			float quadVertices[] = {
				// positions        // texture Coords
				-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			};
			// setup plane VAO
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		}
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
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