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
	SoundSource(tuple <float, float, float> position, ISoundEngine* SoundEngine);
	void update(tuple <float, float, float> positionXYZ, float speed, bool honk);
	void engineStart(bool x);
	void engine(float speed);
	void engineOff();
	void horn(bool x);
	void exhaust();

private:
	ISoundEngine* SoundEngine;
};

#endif