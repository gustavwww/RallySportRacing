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
irrklang::ISoundSource* engineStartSound;
irrklang::ISoundSource* engineOffSound;

	Audio::Audio() {
		// Init sound engine
		SoundEngine = createIrrKlangDevice();

		// Init various sounds
		hornSound = SoundEngine->addSoundSourceFromFile("../RallySportRacing/Audio/ES_Horn Honk Long - SFX Producer.mp3");
		exhaustSound = SoundEngine->addSoundSourceFromFile("../RallySportRacing/Audio/Backfire.mp3");
		engineStartSound = SoundEngine->addSoundSourceFromFile("../RallySportRacing/Audio/engineStart2.mp3");
		engineOffSound = SoundEngine->addSoundSourceFromFile("../RallySportRacing/Audio/engineOff.mp3");

		// Init master volume
		volume = 0.3F;

		// Init engine sound and set volume
		playBackSpeed = 1.0F;
		engineSound = SoundEngine->play2D("../RallySportRacing/Audio/ES_Formula one Race Car 2 - SFX Producer.mp3", true, true, false, ESM_AUTO_DETECT, true);
		engineSound->setVolume(volume/2.0);

		// Init background music and set volume
		//SoundEngine->play2D("../RallySportRacing/Audio/breakout.mp3", true);
		SoundEngine->setSoundVolume(volume);
	}

	void Audio::engine(float speed) {
		playBackSpeed = 1.0 + speed/100;
		engineSound->setPlaybackSpeed(playBackSpeed);
		engineSound->setIsPaused(false);
	}

	void Audio::engineOff() {
		engineSound->setIsPaused(true);
		SoundEngine->play2D(engineOffSound);
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

	void Audio::engineStart(bool x) {
		if (x) {
			if (!SoundEngine->isCurrentlyPlaying(engineStartSound)) {
				SoundEngine->play2D(engineStartSound);
			}
		}
		else {
			SoundEngine->stopAllSoundsOfSoundSource(engineStartSound);
		}
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

