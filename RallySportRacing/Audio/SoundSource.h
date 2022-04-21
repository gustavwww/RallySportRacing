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
	SoundSource(tuple <float, float, float> position);
	void update(tuple <float, float, float> positionXYZ, float speed, string sounds);

private:
	void horn(bool x);
	void exhaust(bool x);
	void engineStart(bool x);
	void engine(bool x, float speed);
	void engineOff(bool x);
};

#endif