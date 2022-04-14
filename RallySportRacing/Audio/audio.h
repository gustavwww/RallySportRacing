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
	void engine();
	void horn(bool x);
	void exhaust();
	void volume(float volume);

private:
	float volumeNr;

	ISoundEngine* SoundEngine;
	irrklang::ISound* engineSound;
	irrklang::ISoundSource* hornSound;
};

#endif