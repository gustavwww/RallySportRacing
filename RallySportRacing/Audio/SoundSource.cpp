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

SoundSource::SoundSource(int ID, tuple<float, float, float> positionXYZ)
{
	positionXYZ = positionXYZ;
	speed = 0.0F;

	// Init engine sound
	playBackSpeed = 1.0F;

	this->soundString = "000";
	
	// Init various sounds
	this->hornSound = Audio::SoundEngine->play2D("../RallySportRacing/Audio/ES_Horn Honk Long - SFX Producer.mp3", true, true, true);
	//this->exhaustSound = Audio::SoundEngine->play2D("../RallySportRacing/Audio/Backfire.mp3", true, true, true);
	//this->engineStartSound = Audio::SoundEngine->play2D("../RallySportRacing/Audio/engineStart2.mp3", true, true, true);
	//this->engineOffSound = Audio::SoundEngine->play2D("../RallySportRacing/Audio/engineOff.mp3", true, true, true);
	
	//this->hornSound = Audio::SoundEngine->addSoundSourceAlias(Audio::hornSound, ID + ": hornSound");
	//this->exhaustSound = Audio::SoundEngine->addSoundSourceAlias(Audio::exhaustSound, ID + ": exhaustSound");
	//this->engineStartSound = Audio::SoundEngine->addSoundSourceAlias(Audio::engineStartSound, ID + ": engineStartSound");
	//this->engineOffSound = Audio::SoundEngine->addSoundSourceAlias(Audio::engineOffSound, ID + ": engineOffSound");



	this->engineSound = Audio::SoundEngine->play2D("../RallySportRacing/Audio/BetterCarAudio.mp3", true, true, true);
}

// Function that updates source
void SoundSource::update(tuple <float, float, float> positionXYZ, float speed, string soundString) {
	//engineSound->setPosition();
	//cout << "Got past here" << endl;

	this->soundString = soundString;

	horn(this->soundString[1] == '1');
	exhaust(this->soundString[2] == '1');

	engineStart(this->soundString[0] == '1');
	engine(this->soundString[0] == '2', speed);
	engineOff(this->soundString[0] == '3');
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
		//cout << "Got to stop" << endl;
	}
}

// Function that plays exhaust sound
void SoundSource::exhaust(bool x) {
	if (x) {
		Audio::SoundEngine->play2D("../RallySportRacing/Audio/Backfire.mp3");
		//if (this->exhaustSound->getIsPaused()) {
		//	this->exhaustSound->setIsPaused(false);
		//}
	}
	/*else {
		if (!this->exhaustSound->getIsPaused()) {
			this->exhaustSound->setIsPaused(true);
		}
	}*/
}

// Function that plays engine start sound
void SoundSource::engineStart(bool x) {
	if (x) {
		Audio::SoundEngine->play2D("../RallySportRacing/Audio/engineStart2.mp3");
		/*if (this->engineSound->getIsPaused() && this->engineStartSound->getIsPaused()) {
			this->engineStartSound->setIsPaused(false);
		}*/
	}
	/*else {
		if (!this->engineStartSound->getIsPaused()) {
			this->engineStartSound->setIsPaused(true);
		}
	}*/
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
		/*if (!this->engineOffSound->getIsPaused()) {
			this->engineOffSound->setIsPaused(false);
		}*/
	}
}

string SoundSource::getSoundString() {
	return this->soundString;
}