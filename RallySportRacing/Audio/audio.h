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
	void engine(float speed);
	void engineOff();
	void horn(bool x);
	void exhaust();
	void volumeUp();
	void volumeDown();
	void volumeSet(float v);
	void Audio::engineStart(bool x);

private:
	float volume;
	float playBackSpeed;

	ISoundEngine* SoundEngine;
	irrklang::ISound* engineSound;
	irrklang::ISoundSource* hornSound;
	irrklang::ISoundSource* engineStartSound;
	irrklang::ISoundSource* engineOffSound;
};

#endif