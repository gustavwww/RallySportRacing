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
	~Audio();

	void volumeUp();
	void volumeDown();
	void volumeSet(float v);

	void createSoundSource(int ID, glm::vec3 positionVec3);
	void updateSoundSource(int ID, glm::vec3 positionVec3, glm::vec3 velPerFrame, float speed, string soundString);
	void removeSoundSource(int ID);
	void setListenerParameters(glm::vec3 positionVec3, glm::vec3 direction, glm::vec3 velPerFrame, float speed);
	string getSoundString(int ID);
	void playSourcelessSounds(glm::vec3 positionVec3);
	void playButtonPressSound();
	void playCheckpointSound();
	void playFinishSound();
	void playStartSound();
	void playRainSound(bool playRain);

	static ISoundEngine* SoundEngine;

private:
	static Audio* instance;
	irrklang::vec3df getVelMetersPerSec(glm::vec3 velPerFrame, float speedKmPerh);
	void rain(glm::vec3 position);
	irrklang::vec3df glmToirrklangVec(glm::vec3 inVec);
	irrklang::vec3df glmToirrklangVec(glm::vec3 inVec, int scalar);

	int distanceScalar;
	bool playRain;
	irrklang::ISound* rainSound;
};

#endif