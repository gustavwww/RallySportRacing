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

float speed;
float playBackSpeed;

SoundSource::SoundSource(int ID, tuple<float, float, float> positionXYZ)
{
	irrklang::vec3df position(get<0>(positionXYZ), get<1>(positionXYZ), get<2>(positionXYZ));
	speed = 0.0F;

	// Init engine sound
	playBackSpeed = 1.0F;

	this->soundString = "000";
	startSoundTimer = 0;
	
	// Init looping sounds
	this->hornSound = Audio::SoundEngine->play3D("../RallySportRacing/Audio/ES_Horn Honk Long - SFX Producer.mp3", position, true, true, true);
	this->engineSound = Audio::SoundEngine->play3D("../RallySportRacing/Audio/BetterCarAudio.mp3",position,  true, true, true);
	//this->engineSound = Audio::SoundEngine->play2D("../RallySportRacing/Audio/BetterCarAudio.mp3", true, true, true);

	//this->engineSound->setVolume(2);
}

// Destructor of SoundSource
SoundSource::~SoundSource()	{
	if (this->hornSound) {
		this->hornSound->stop();
		this->hornSound->drop();
		this->hornSound = 0;
	}
	if (this->engineSound) {
		this->engineSound->stop();
		this->engineSound->drop();
		this->engineSound = 0;
	}
}

// Function that updates source
void SoundSource::update(tuple <float, float, float> positionXYZ, tuple <float, float, float> velPerFrame, float speed, string soundString) {
	irrklang::vec3df position(get<0>(positionXYZ), get<1>(positionXYZ), get<2>(positionXYZ));

	// Get velocity vector in m/s
	irrklang::vec3df velMetersPerSecond = Audio::getVelMetersPerSec(velPerFrame, speed);

	this->soundString = soundString;

	horn(this->soundString[1] == '1', position, velMetersPerSecond);
	exhaust(this->soundString[2] == '1', position);

	engineStart(this->soundString[0] == '1', position);
	engine(this->soundString[0] == '2', speed, position);
	engineOff(this->soundString[0] == '3', position);
}

// Function that plays horn sound
void SoundSource::horn(bool x, irrklang::vec3df position, irrklang::vec3df velMetersPerSec) {
	this->hornSound->setPosition(position);
	this->hornSound->setVelocity(velMetersPerSec);

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
void SoundSource::exhaust(bool x, irrklang::vec3df position) {
	if (x) {
		Audio::SoundEngine->play3D("../RallySportRacing/Audio/Backfire.mp3", position);
	}
}

// Function that plays engine start sound
void SoundSource::engineStart(bool x, irrklang::vec3df position) {
	if (x && startSoundTimer == 0) {
		Audio::SoundEngine->play3D("../RallySportRacing/Audio/engineStart2.mp3", position);
		startSoundTimer = 120;
	}
	if (startSoundTimer > 0) {
		startSoundTimer--;
	}
}

// Function that plays engine sound
void SoundSource::engine(bool x, float speed, irrklang::vec3df position) {
	this->engineSound->setPosition(position);
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
void SoundSource::engineOff(bool x, irrklang::vec3df position) {
	if (x) {
		Audio::SoundEngine->play3D("../RallySportRacing/Audio/engineOff.mp3", position);
	}
}

string SoundSource::getSoundString() {
	return this->soundString;
}