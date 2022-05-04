#ifndef AUDIO
#define AUDIO

#include <iostream>
#include <string>
#include <windows.h>
#include "../../External/irrKlang/include/irrKlang.h"
#include <glm/glm.hpp>
#include <map>
#include "SoundSource.h"

using namespace std;
using namespace irrklang;

class Audio {
public:
	static Audio* Instance();
	Audio();

	void volumeUp();
	void volumeDown();
	void volumeSet(float v);

	void createSoundSource(int ID, glm::vec3 position);
	void updateSoundSource(int ID, glm::vec3 position, glm::vec3 velPerFrame, float speed, string sounds);
	void removeSoundSource(int ID);
	string getSoundString(int ID);
	void setListenerParameters(glm::vec3 positionXYZ, glm::vec3 direction, glm::vec3 velPerFrame, float speed);
	static irrklang::vec3df getVelMetersPerSec(glm::vec3 velPerFrame, float speedKmPerh);


	static ISoundEngine* SoundEngine;

	static int distanceScalar;

private:
	static Audio* instance;
};

#endif