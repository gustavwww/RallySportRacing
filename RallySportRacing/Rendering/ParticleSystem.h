#ifndef PARTICLESYSTEM
#define PARTICLESYSTEM

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

using namespace std;

namespace Rendering {

	struct Particle
	{
		glm::vec3 pos;
		glm::vec3 velocity;
		float lifeLength;
		float lifetime = 0;
		bool active = false;
	};

	class ParticleSystem {
	public:
		vector<Particle> particles;
		unsigned int nrActiveParticles = 0;
		unsigned int maxSize;
		unsigned int texture;

		unsigned int VAO, VBO;

		ParticleSystem(unsigned int maxSize, unsigned int texture);
		ParticleSystem() {

		};
		~ParticleSystem();

		void emitParticle(glm::vec3 pos, glm::vec3 velocity, float lifeLength);
		void updateParticles();
		void render(GLint programID, glm::mat4 projectionMatrix, glm::mat4 viewMatrix, float screenWidth, float screenHeight);


	private:
		vector<glm::vec4> extractData(glm::mat4 viewMatrix);
		void handleData(vector<glm::vec4> data);

		void setup();
		void renderParticleSystem(GLint programID, glm::mat4 projectionMatrix, float screenWidth, float screenHeight);
	};
}
#endif