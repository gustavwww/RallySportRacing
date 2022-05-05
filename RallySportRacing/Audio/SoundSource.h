#ifndef SOUNDSOURCE
#define SOUNDSOURCE
#include "audio.h"

#include <iostream>
#include <string>
#include <windows.h>
#include "../../External/irrKlang/include/irrKlang.h"
#include <glm/glm.hpp>

using namespace std;
using namespace irrklang;

class SoundSource {
public:
	SoundSource(int ID, glm::vec3 positionVec3);
	~SoundSource();
	void update(glm::vec3 positionVec3, glm::vec3 velPerFrame, float speed, string sounds);
	void setPlayRain(bool playRain);
	string getSoundString();

private:
	void horn(bool x, irrklang::vec3df position, irrklang::vec3df velMetersPerSec);
	void exhaust(bool x, irrklang::vec3df position);
	void rain(bool x, irrklang::vec3df position);
	void engineStart(bool x, irrklang::vec3df position);
	void terrain(bool x, float speed, irrklang::vec3df position, irrklang::vec3df velMetersPerSec, char terrain);
	void engine(bool x, float speed, irrklang::vec3df position, irrklang::vec3df velMetersPerSec);
	void engineOff(bool x, irrklang::vec3df position);

	string soundString;
	int startSoundTimer;
	bool playRain;

	irrklang::ISound* hornSound;
	irrklang::ISound* exhaustSound;
	irrklang::ISound* rainSound;
	irrklang::ISound* engineStartSound;
	irrklang::ISound* engineOffSound;
	irrklang::ISound* engineSound;
	irrklang::ISound* dirtSound;
	irrklang::ISound* pavementSound;
};

#endif