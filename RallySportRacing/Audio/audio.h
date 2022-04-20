#ifndef AUDIO
#define AUDIO

#include <iostream>
#include <string>
#include <windows.h>
#include "../../External/irrKlang/include/irrKlang.h"

using namespace std;
using namespace irrklang;

class Audio {
public:
	Audio();

	void volumeUp();
	void volumeDown();
	void volumeSet(float v);

	void createSoundSource(string ID, tuple <float, float, float> position);
	void updateSoundSource(string ID, tuple <float, float, float> position,float speed, string sounds);
	void removeSoundSource(string ID);
};

#endif