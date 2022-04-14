#include "audio.h"
#include <iostream>
#include <string>
#include <windows.h>
#include "../../External/irrKlang/include/irrKlang.h"

using namespace std;
using namespace irrklang;

#pragma comment(lib, "../../External/irrKlang/irrKlang.lib")

float volume;
float playBackSpeed;

ISoundEngine* SoundEngine;
irrklang::ISound* engineSound;

irrklang::ISoundSource* hornSound;
irrklang::ISoundSource* exhaustSound;

	Audio::Audio() {
		SoundEngine = createIrrKlangDevice();
		hornSound = SoundEngine->addSoundSourceFromFile("../RallySportRacing/Audio/ES_Horn Honk Long - SFX Producer.mp3");
		exhaustSound = SoundEngine->addSoundSourceFromFile("../RallySportRacing/Audio/Backfire.mp3");
		volume = 1.0F;
		playBackSpeed = 1.0F;

		engineSound = SoundEngine->play2D("../RallySportRacing/Audio/ES_Formula one Race Car 2 - SFX Producer.mp3", true, false, false, ESM_AUTO_DETECT, true);
		engineSound->setVolume(0.3F);

		SoundEngine->play2D("../RallySportRacing/Audio/breakout.mp3", true);
	}

	void Audio::engine(float speed) {
		playBackSpeed = 1.0 + speed/100;
		engineSound->setPlaybackSpeed(playBackSpeed);
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
		SoundEngine->play2D(exhaustSound);
	}

	void Audio::volumeUp() {
		if (volume < 1.0F) {
			volume = volume + 0.05F;
		}
		SoundEngine->setSoundVolume(volume);
	}

	void Audio::volumeDown() {
		if (volume > 0.0F) {
			volume = volume - 0.05F;
		}
		SoundEngine->setSoundVolume(volume);
	}

	void Audio::volumeSet(float v) {
		volume = v;
		SoundEngine->setSoundVolume(volume);
	}

