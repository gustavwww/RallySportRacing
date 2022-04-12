#include "ParticleSystem.h"
#include <GL/glew.h>
#include <algorithm>
#include <Utils/GameTimer.h>

using namespace Utils;

namespace Rendering {

	GameTimer* gameTimer;

	ParticleSystem::ParticleSystem(unsigned int maxSize, unsigned int texture) {
		this->maxSize = maxSize;
		this->texture = texture;
		setup();

		//Populate particles.
		particles.resize(maxSize);
	}

	void ParticleSystem::setup() {
		gameTimer = GameTimer::Instance();
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, ParticleSystem::maxSize * sizeof(unsigned int), nullptr, GL_STATIC_DRAW);
	}

	void ParticleSystem::emitParticle(glm::vec3 pos, glm::vec3 velocity, float lifeLength) {
		if (nrActiveParticles < maxSize){
			particles.at(nrActiveParticles).pos = pos;
			particles.at(nrActiveParticles).velocity = velocity;
			particles.at(nrActiveParticles).lifeLength = lifeLength;
			particles.at(nrActiveParticles).lifetime = 0;
			nrActiveParticles++;
		}
	}

	//Deactivate dead particles and updates data for alive particles.
	void ParticleSystem::updateParticles() {
		for (int i = 0; i < nrActiveParticles; i++){

			if (particles.at(i).lifetime > particles.at(i).lifeLength) {
				nrActiveParticles -= 1;
				std::swap(particles.at(i), particles.at(nrActiveParticles));

				//Alternativ swapping
				//Particle temp = particles.at(i);
				//particles.at(i) = particles.at(nrActiveParticles);
				//particles.at(nrActiveParticles) = temp;
			}
			particles.at(i).pos += gameTimer->getDeltaTime() * particles.at(i).velocity;
			particles.at(i).lifetime += gameTimer->getDeltaTime();
		}
	}

	void ParticleSystem::render(GLint programID, glm::mat4 projectionMatrix, glm::mat4 viewMatrix, float screenWidth, float screenHeight){
		updateParticles();
		handleData(extractData(viewMatrix));
		renderParticleSystem(programID, projectionMatrix, screenWidth, screenHeight);
	}

	//Collect, convert and sort data from particle system.
	vector<glm::vec4> ParticleSystem::extractData(glm::mat4 viewMatrix) {
		vector<glm::vec4> data;

		//Add data vector containing position in view space and lifetime for each active particle in the system.
		for (int i = 0; i < nrActiveParticles; i++) {
			glm::vec3 posViewSpace = glm::vec3(viewMatrix * glm::vec4(particles.at(i).pos, 1));
			data.push_back(glm::vec4(posViewSpace, particles.at(i).lifetime));
		}

		return data;
	}

	//Add data for every active particle to buffer.
	void ParticleSystem::handleData(vector<glm::vec4> data) {
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec4) * nrActiveParticles, data.data());

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	}

	//Make all neseccery gl commands before a single rendering call.
	void ParticleSystem::renderParticleSystem(GLint programID, glm::mat4 projectionMatrix, float screenWidth, float screenHeight) {
		glUseProgram(programID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glUniform1i(glGetUniformLocation(programID, "textureIn"), 0);
		glUniformMatrix4fv(glGetUniformLocation(programID, "projMat"), 1, GL_FALSE, &projectionMatrix[0][0]);
		glUniform1f(glGetUniformLocation(programID, "screen_x"), screenWidth);
		glUniform1f(glGetUniformLocation(programID, "screen_y"), screenHeight);

		//Enable and set blending function. 
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//Change depth function and depthMask.
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_FALSE);

		glEnable(GL_PROGRAM_POINT_SIZE);
		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, nrActiveParticles);

		//Reset depth function and depthMask.
		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);
	}

	ParticleSystem::~ParticleSystem() {
	}

}