#include "ParticleSystem.h"
#include <GL/glew.h>
#include <algorithm>

void ParticleSystem::setup() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, ParticleSystem::maxSize * sizeof(unsigned int), nullptr, GL_STATIC_DRAW);
}

void ParticleSystem::createParticle(float lifeLength, glm::vec3 velocity, glm::vec3 pos) {
	Particle particle;
	particle.lifetime = 0;
	particle.lifeLength = lifeLength;
	particle.velocity = velocity;
	particle.pos = pos;

	addParticleToParticleSystem(particle);
}

void ParticleSystem::addParticleToParticleSystem(Particle particle) {
	if (particles.size() < maxSize) {
		particles.push_back(particle);
	}
}

void ParticleSystem::killParticle(int index) {
	particles.erase(particles.begin() + index);
}

//MIGHT NEED TO CHANGE THIS TO ONLY HAVE 1 LOOP.
void ParticleSystem::updateParticles(float deltaTime) {

	//Remove dead particles.
	for (int i = 0; i < particles.size(); i++) {
		if (particles[i].lifetime > particles[i].lifeLength) {
			killParticle(i);
		}
	}

	for (int i = 0; i < particles.size(); i++) {
		particles[i].pos += deltaTime * particles[i].velocity;
		particles[i].lifetime += deltaTime;
	}

}

void ParticleSystem::renderParticleSystem(GLint programID, glm::mat4 projectionMatrix, glm::mat4 viewMatrix, float screenWidth, float screenHeight, unsigned int texture) {
	handleData(extractData(viewMatrix));
	render(programID, projectionMatrix, screenWidth, screenHeight, texture);
}

//Collect, convert and sort data from particle system.
vector<glm::vec4> ParticleSystem::extractData(glm::mat4 viewMatrix) {
	vector<glm::vec4> data;

	//Add data vector containing position in view space and lifetime for each particle in the system.
	for (int i = 0; i < particles.size(); i++) {
		glm::vec3 posViewSpace = glm::vec3(viewMatrix * glm::vec4(particles[i].pos, 1));
		data.push_back(glm::vec4(posViewSpace, particles[i].lifetime));
	}

	sort(data.begin(), next(data.begin(), particles.size()), [](const glm::vec4& lhs, const glm::vec4& rhs) { return lhs.z < rhs.z; });

	return data;
}

//Add data to buffer.
void ParticleSystem::handleData(vector<glm::vec4> data) {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec4) * particles.size(), data.data());


	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0 , 0);
}

//ToDo CHANGE AND COMMENT THIS
void ParticleSystem::render(GLint programID, glm::mat4 projectionMatrix, float screenWidth, float screenHeight, unsigned int texture) {
	glUseProgram(programID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glUniform1i(glGetUniformLocation(programID, "textureIn"), 0);
	glUniformMatrix4fv(glGetUniformLocation(programID, "projMat"), 1, GL_FALSE, &projectionMatrix[0][0]);
	glUniform1f(glGetUniformLocation(programID, "screen_x"), screenWidth);
	glUniform1f(glGetUniformLocation(programID, "screen_y"), screenHeight);

	glEnable(GL_PROGRAM_POINT_SIZE);

	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, particles.size());
}