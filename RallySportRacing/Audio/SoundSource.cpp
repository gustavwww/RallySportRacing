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

tuple<float, float, float> positionXYZ;

float speed;
float playBackSpeed;

SoundSource::SoundSource(tuple<float, float, float> positionXYZ)
{
	positionXYZ = positionXYZ;
	speed = 0.0F;

	// Init engine sound
	playBackSpeed = 1.0F;

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
		if (!Audio::SoundEngine->isCurrentlyPlaying(Audio::hornSound)) {
			Audio::SoundEngine->play2D(Audio::hornSound);
		}
	}
	else {
		Audio::SoundEngine->stopAllSoundsOfSoundSource(Audio::hornSound);
	}
}

// Function that plays exhaust sound
void SoundSource::exhaust(bool x) {
	if (x) {
		if (!Audio::SoundEngine->isCurrentlyPlaying(Audio::exhaustSound)) {
			Audio::SoundEngine->play2D(Audio::exhaustSound);
		}
	}
}

// Function that plays engine start sound
void SoundSource::engineStart(bool x) {
	if (x) {
		if (!Audio::SoundEngine->isCurrentlyPlaying(Audio::engineStartSound)) {
			Audio::SoundEngine->play2D(Audio::engineStartSound);
		}
	}
}

// Function that plays engine sound
void SoundSource::engine(bool x, float speed) {
	if (x) {
		if (Audio::engineSound->getIsPaused()) {
			Audio::engineSound->setIsPaused(false);
		}
	}
	playBackSpeed = 1.0 + abs(speed) / 100;
	Audio::engineSound->setPlaybackSpeed(playBackSpeed);
}

// Function that plays engine off sound
void SoundSource::engineOff(bool x) {
	if (x) {
		Audio::engineSound->setIsPaused(true);
		Audio::SoundEngine->play2D(Audio::engineOffSound);
	}
}