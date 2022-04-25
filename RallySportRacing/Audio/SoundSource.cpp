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

int startSoundTimer = 0;

// Constructor of SoundSource
SoundSource::SoundSource(int ID, tuple<float, float, float> positionXYZ)
{
	positionXYZ = positionXYZ;
	speed = 0.0F;

	// Init engine sound
	playBackSpeed = 1.0F;

	startSoundTimer = 0;

	this->soundString = "000";
	
	// Init looping sounds
	this->hornSound = Audio::SoundEngine->play2D("../RallySportRacing/Audio/ES_Horn Honk Long - SFX Producer.mp3", true, true, true);
	this->engineSound = Audio::SoundEngine->play2D("../RallySportRacing/Audio/BetterCarAudio.mp3", true, true, true);
}

// Function that updates source
void SoundSource::update(tuple <float, float, float> positionXYZ, float speed, string soundString) {
	//engineSound->setPosition();

	this->soundString = soundString;

	horn(this->soundString[1] == '1');
	exhaust(this->soundString[2] == '1');

	engineStart(this->soundString[0] == '1');
	engine(this->soundString[0] == '2', speed);
	engineOff(this->soundString[0] == '3');
}

// Function that removes sound source and sound pointers
void SoundSource::removeSoundSource() {
	if(this->hornSound) {
		this->hornSound->drop();
		this->hornSound = 0;
	}
	if (this->engineSound) {
		this->engineSound->drop();
		this->engineSound = 0;
	}
}

// Function that returns soundString
string SoundSource::getSoundString() {
	return this->soundString;
}

// Function that plays horn sound
void SoundSource::horn(bool x) {

	if (x) {
		if (this->hornSound->getIsPaused()) {
			this->hornSound->setIsPaused(false);
		}
	}
	else {
		this->hornSound->setIsPaused(true);
	}
}

// Function that plays exhaust sound
void SoundSource::exhaust(bool x) {
	if (x) {
		Audio::SoundEngine->play2D("../RallySportRacing/Audio/Backfire.mp3");
	}
}

// Function that plays engine start sound
void SoundSource::engineStart(bool x) {
	if (x && startSoundTimer == 0) {
		Audio::SoundEngine->play2D("../RallySportRacing/Audio/engineStart2.mp3");
		startSoundTimer = 120;
	}
	if (startSoundTimer > 0) {
		startSoundTimer--;
	}
}

// Function that plays engine sound
void SoundSource::engine(bool x, float speed) {
	if (x) {
		if (this->engineSound->getIsPaused()) {
			this->engineSound->setIsPaused(false);
		}
	}
	else {
		if (!this->engineSound->getIsPaused()) {
			this->engineSound->setIsPaused(true);
		}
	}
	playBackSpeed = 1.0 + abs(speed) / 100;
	this->engineSound->setPlaybackSpeed(playBackSpeed);
}

// Function that plays engine off sound
void SoundSource::engineOff(bool x) {
	if (x) {
		Audio::SoundEngine->play2D("../RallySportRacing/Audio/engineOff.mp3");
	}
}