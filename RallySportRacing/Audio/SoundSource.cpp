#include "SoundSource.h"
#include "audio.h"

#include <iostream>
#include <string>
#include <windows.h>
#include "../../External/irrKlang/include/irrKlang.h"

#include<tuple>

using namespace std;
using namespace irrklang;

#pragma comment(lib, "../../External/irrKlang/irrKlang.lib")

irrklang::ISoundSource* hornSound;
irrklang::ISoundSource* exhaustSound;
irrklang::ISoundSource* engineStartSound;
irrklang::ISoundSource* engineOffSound;

irrklang::ISound* engineSound;

tuple<float, float, float> positionXYZ;

float speed;
float playBackSpeed;

SoundSource::SoundSource(tuple<float, float, float> positionXYZ, ISoundEngine* SoundEngine)
{

	// Pass on SoundEngine
	this->SoundEngine = SoundEngine;

	positionXYZ = positionXYZ;
	speed = 0.0F;

	// Init various sounds
	hornSound = this->SoundEngine->addSoundSourceFromFile("../RallySportRacing/Audio/ES_Horn Honk Long - SFX Producer.mp3");
	exhaustSound = this->SoundEngine->addSoundSourceFromFile("../RallySportRacing/Audio/Backfire.mp3");
	engineStartSound = this->SoundEngine->addSoundSourceFromFile("../RallySportRacing/Audio/engineStart2.mp3");
	engineOffSound = this->SoundEngine->addSoundSourceFromFile("../RallySportRacing/Audio/engineOff.mp3");

	// Init engine sound and set volume
	playBackSpeed = 1.0F;
	engineSound = this->SoundEngine->play2D("../RallySportRacing/Audio/BetterCarAudio.mp3", true, true, true);

}

// Function that updates source
void SoundSource::update(tuple <float, float, float> positionXYZ, float speed, string sounds) {
	//engineSound->setPosition();

	horn(sounds[1] == '1');
	exhaust(sounds[2] == '1');

	engineStart(sounds[0] == '1');
	engine(sounds[0] == '2', speed);
	engineOff(sounds[0] == '3');
}

// Function that plays horn sound
void SoundSource::horn(bool x) {

	if (x) {
		if (!this->SoundEngine->isCurrentlyPlaying(hornSound)) {
			this->SoundEngine->play2D(hornSound);
		}
	}
	else {
		this->SoundEngine->stopAllSoundsOfSoundSource(hornSound);
	}
}

// Function that plays exhaust sound
void SoundSource::exhaust(bool x) {
	if (x) {
		if (!this->SoundEngine->isCurrentlyPlaying(exhaustSound)) {
			this->SoundEngine->play2D(exhaustSound);
		}
	}
}

// Function that plays engine start sound
void SoundSource::engineStart(bool x) {
	if (x) {
		if (!this->SoundEngine->isCurrentlyPlaying(engineStartSound)) {
			this->SoundEngine->play2D(engineStartSound);
		}
	}
}

// Function that plays engine sound
void SoundSource::engine(bool x, float speed) {
	if (x) {
		if (engineSound->getIsPaused()) {
			engineSound->setIsPaused(false);
		}
	}
	playBackSpeed = 1.0 + speed / 100;
	engineSound->setPlaybackSpeed(playBackSpeed);
}

// Function that plays engine off sound
void SoundSource::engineOff(bool x) {
	if (x) {
		engineSound->setIsPaused(true);
		this->SoundEngine->play2D(engineOffSound);
	}
}