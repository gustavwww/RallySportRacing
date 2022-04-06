#ifndef PARTICLESYSTEM
#define PARTICLESYSTEM

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

using namespace std;

struct Particle
{
	float lifetime;
	float lifeLength;
	glm::vec3 velocity;
	glm::vec3 pos;
};

class ParticleSystem {
public:
	vector<Particle> particles;
	unsigned int maxSize;

	unsigned int VAO, VBO;

	ParticleSystem(unsigned int maxSize) : maxSize(maxSize) {
		setup();
	}

	void createParticle(float lifeLength, glm::vec3 velocity, glm::vec3 pos);
	void addParticleToParticleSystem(Particle particle);
	void updateParticles(float deltaTime);
	void killParticle(int index);
	void renderParticleSystem(GLint programID, glm::mat4 projectionMatrix, glm::mat4 viewMatrix, float screenWidth, float screenHeight, unsigned int texture);
	void handleData(vector<glm::vec4> data);

private:
	void setup();
	vector<glm::vec4> extractData(glm::mat4 viewMatrix);
	void render(GLint programID, glm::mat4 projectionMatrix, float screenWidth, float screenHeight, unsigned int texture);
};
#endif