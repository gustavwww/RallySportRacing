#ifndef SOUNDSOURCE
#define SOUNDSOURCE

#include <iostream>
#include <string>
#include <windows.h>
#include "../../External/irrKlang/include/irrKlang.h"

using namespace std;
using namespace irrklang;

class SoundSource {
public:
	SoundSource(int ID, tuple <float, float, float> position);
	~SoundSource();
	void update(tuple <float, float, float> positionXYZ, tuple <float, float, float> velPerFrame, float speed, string sounds);
	string getSoundString();

private:
	void horn(bool x, irrklang::vec3df position, irrklang::vec3df velMetersPerSec);
	void exhaust(bool x, irrklang::vec3df position);
	void engineStart(bool x, irrklang::vec3df position);
	void engine(bool x, float speed, irrklang::vec3df position);
	void engineOff(bool x, irrklang::vec3df position);

	string soundString;
	int startSoundTimer;

	irrklang::ISound* hornSound;
	irrklang::ISound* exhaustSound;
	irrklang::ISound* engineStartSound;
	irrklang::ISound* engineOffSound;
	irrklang::ISound* engineSound;
};

#endif