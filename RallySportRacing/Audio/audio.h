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
	static Audio* Instance();
	Audio();

	void volumeUp();
	void volumeDown();
	void volumeSet(float v);

	void createSoundSource(int ID, tuple <float, float, float> position);
	void updateSoundSource(int ID, tuple <float, float, float> position,float speed, string sounds);
	void removeSoundSource(int ID);
	string getSoundString(int ID);

	static ISoundEngine* SoundEngine;

private:
	static Audio* instance;
};

#endif