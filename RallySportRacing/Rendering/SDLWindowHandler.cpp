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
#include "FrameBufferObject.h"
#include "ShadowBox.h"


using namespace std;

namespace Rendering {

	//Camera
	float nearPlane = 0.1f, farPlane = 100.0f, fov = 45.0f;

	//Light
	glm::vec3 lightColor = glm::vec3(1.f, 1.f, 1.f);
	glm::vec4 lightPos = glm::vec4(1.0f, 40.0f, 1.0f, 1.0f);
	float lightIntensity = 5000.0f;
	float envMultiplier = 1.5f;
	
	//Shadow maps.
	FboInfo shadowMapFB;
	int shadowMapResolution = 8000;
	float polygonFactor = 3.5f, polygonUnits = 1.0f;

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
		glm::mat4 projection = glm::perspective(glm::radians(fov), (float)width / (float)height, nearPlane, farPlane);

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
			ImGui::DragFloat("light intensity", &lightIntensity);
			ImGui::DragFloat("polygon factor", &polygonFactor);
			ImGui::DragFloat("polygon units", &polygonUnits);

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
			
			//Shadow maps creation.
			//glm::mat4 lightViewMatrix = lookAt(glm::vec3(lightPos), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
			glm::mat4 lightViewMatrix = glm::lookAt(glm::vec3(lightPos) + camPosition, camPosition, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 lightProjMatrix = glm::ortho(-50.f, 50.f, -50.f, 50.f, nearPlane, farPlane);
			//glm::mat4 lightProjMatrix = getLightProjection(lightViewMatrix);
			//glm::mat4 lightProjMatrix = glm::perspective(glm::radians(45.0f), float(width/height), 25.0f, 1000.0f);

			glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFB.framebufferId);
			glViewport(0, 0, shadowMapFB.width, shadowMapFB.height);
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(polygonFactor, polygonUnits);

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
			glUniformMatrix4fv(glGetUniformLocation(programID, "lightMatrix"), 1, GL_FALSE, &lightMatrix[0][0]);


			for (Model* m : models) {
				m->render(projection, view, programID);
			}

			for (ParticleSystem* p : particleSystems) {
				p->render(particleProgramID, projection, view, width, height);
			}
		
			glUseProgram(shadowMapID);
			ImGui::Image((ImTextureID)shadowMapFB.colorTextureTarget, ImVec2(700, 700));
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

	//OLD METHODS SHOULD BE REMOVED
	/*glm::mat4 SDLWindowHandler::getLightProjection(glm::mat4 lightView) {
		float aspectRatio = float(width / height);
		
		//Calculate width and height of far plane and near plane.
		
		float widthNear = (nearPlane * tan(glm::radians(fov)));
		float heightNear = widthNear / (width / height);
		float widthFar = (100 * tan(glm::radians(fov)));
		float heightFar = widthFar / (width / height);
		
		//float heightNear = 2 * tan(glm::radians(fov) / 2) * nearPlane;
		//float widthNear = heightNear * aspectRatio;
		//float heightFar = 2 * tan(glm::radians(fov) / 2) * farPlane;
		//float widthFar = heightFar * aspectRatio;
		
		//Calculate right vector and normalize it.
		glm::vec3 camOrientationTEST = normalize(camOrientation);
		glm::vec3 camDirectionTEST = normalize(camDirection);
		glm::vec3 camRight = cross(camOrientation, camDirection);
		camRight = normalize(camRight);
		
		//Find the center for points for far and near plane.
		glm::vec3 centerNear = camPosition + camDirectionTEST * nearPlane;
		glm::vec3 centerFar = camPosition + camDirectionTEST * farPlane;

		//Find the 8 corners of the viewFrustrum.
		glm::vec3 topLeftNear = centerNear + (camOrientationTEST * (heightNear / 2)) - (camRight * (widthNear / 2));
		glm::vec3 topRightNear = centerNear + (camOrientationTEST * (heightNear / 2)) + (camRight * (widthNear / 2));
		glm::vec3 botLeftNear = centerNear - (camOrientationTEST * (heightNear / 2)) - (camRight * (widthNear / 2));
		glm::vec3 botRightNear = centerNear - (camOrientationTEST * (heightNear / 2)) + (camRight * (widthNear / 2));
		
		glm::vec3 topLeftFar = centerFar + (camOrientationTEST * (heightFar / 2)) - (camRight * (widthFar / 2));
		glm::vec3 topRightFar = centerFar + (camOrientationTEST * (heightFar / 2)) + (camRight * (widthFar / 2));
		glm::vec3 botLeftFar = centerFar - (camOrientationTEST * (heightFar / 2)) - (camRight * (widthFar / 2));
		glm::vec3 botRightFar = centerFar - (camOrientationTEST * (heightFar / 2)) + (camRight * (widthFar / 2));

		//Convert corners to lightSpace.
		vector<glm::vec3> frustumInLightSpace{
			lightView * glm::vec4(topLeftNear, 1.0),
			lightView * glm::vec4(topRightNear, 1.0),
			lightView * glm::vec4(botLeftNear, 1.0),
			lightView * glm::vec4(botRightNear, 1.0),
			lightView * glm::vec4(topLeftFar, 1.0),
			lightView * glm::vec4(topRightFar, 1.0),
			lightView * glm::vec4(botLeftFar, 1.0),
			lightView * glm::vec4(botRightFar, 1.0)
		};

		//Find max and min points.
		glm::vec3 min{ INFINITY, INFINITY, INFINITY };
		glm::vec3 max{ -INFINITY, -INFINITY, -INFINITY };
		for (unsigned int i = 0; i < frustumInLightSpace.size(); i++)
		{
			//Update min value.
			if (frustumInLightSpace[i].x < min.x)
				min.x = frustumInLightSpace[i].x;
			if (frustumInLightSpace[i].y < min.y)
				min.y = frustumInLightSpace[i].y;
			if (frustumInLightSpace[i].z < min.z)
				min.z = frustumInLightSpace[i].z;

			//Update max value.
			if (frustumInLightSpace[i].x > max.x)
				max.x = frustumInLightSpace[i].x;
			if (frustumInLightSpace[i].y > max.y)
				max.y = frustumInLightSpace[i].y;
			if (frustumInLightSpace[i].z > max.z)
				max.z = frustumInLightSpace[i].z;
		}

		float minX = min.x;
		float maxX = max.x;
		float minY = min.y;
		float maxY = max.y;
		
		//Change sign since pos z is towards the camera.
		float minZ = -min.z;
		float maxZ = -max.z;
		
		

		return glm::ortho(minX, maxX, minY, maxY, maxZ, minZ);
	}*/

	/*glm::mat4 SDLWindowHandler::getLightView(glm::vec3 lightDirection, glm::vec3 centerPoint) {
		glm::vec3 lightDir = normalize(lightDirection);
		
		float pitch = cos()
	}*/

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