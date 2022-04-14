#include "audio.h"
#include <iostream>
#include <string>
#include <windows.h>
#include "../../External/irrKlang/include/irrKlang.h"

using namespace std;
using namespace irrklang;

#pragma comment(lib, "../../External/irrKlang/irrKlang.lib")

float volumeNr;

ISoundEngine* SoundEngine;
irrklang::ISound* engineSound;

irrklang::ISoundSource* hornSound;
irrklang::ISoundSource* exhaustSound;

	Audio::Audio() {
		SoundEngine = createIrrKlangDevice();
		 hornSound = SoundEngine->addSoundSourceFromFile("../RallySportRacing/Audio/ES_Horn Honk Long - SFX Producer.mp3");

		//SoundEngine->play2D("../RallySportRacing/Audio/breakout.mp3", true);
	}

	void Audio::engine() {
		engineSound = SoundEngine->play2D("../RallySportRacing/Audio/ES_Formula one Race Car 2 - SFX Producer.mp3", true, false, false, ESM_AUTO_DETECT, true);
		
		if (engineSound)
		{
			irrklang::ISoundEffectControl* fx = engineSound->getSoundEffectControl();
			if (fx)
			{
				// enable the echo sound effect for this sound
				fx->enableEchoSoundEffect();
			}
		}
	}

	void Audio::horn(bool x) {

		if (x) {
			if (!SoundEngine->isCurrentlyPlaying(hornSound)) {
				SoundEngine->play2D(hornSound);
			}
		}
		else {
			SoundEngine->stopAllSoundsOfSoundSource(hornSound);
		}
	}

	void Audio::exhaust() {

	}

	void Audio::volume(float volume) {
		SoundEngine->setSoundVolume(volume);
	}
