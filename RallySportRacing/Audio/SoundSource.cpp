#include "SoundSource.h"


using namespace std;
using namespace irrklang;

#pragma comment(lib, "../../External/irrKlang/irrKlang.lib")

float speed;

int engineFade;
int sPressed;

SoundSource::SoundSource(int ID, irrklang::vec3df position) {
	// Init engine sound
	speed = 0.0F;

	this->soundString = "00000";
	startSoundTimer = 0;
	
	// Init looping sounds
	this->hornSound = Audio::SoundEngine->play3D("../RallySportRacing/Audio/ES_Horn Honk Long - SFX Producer.wav", position, true, true, true);


	this->engineSound = Audio::SoundEngine->play3D("../RallySportRacing/Audio/low.wav",position,  true, true, true);
	this->engineHighAcc = Audio::SoundEngine->play3D("../RallySportRacing/Audio/high ACC.wav", position, true, true, true);
	this->engineHighDec = Audio::SoundEngine->play3D("../RallySportRacing/Audio/high Deacc.wav", position, true, true, true);

	this->pavementSound = Audio::SoundEngine->play3D("../RallySportRacing/Audio/TerrainPavement2.wav", position, true, true, true);
	this->dirtSound = Audio::SoundEngine->play3D("../RallySportRacing/Audio/TerrainDirt.wav", position, true, true, true);
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
	if (this->dirtSound) {
		this->dirtSound->stop();
		this->dirtSound->drop();
		this->dirtSound = 0;
	}
	if (this->pavementSound) {
		this->pavementSound->stop();
		this->pavementSound->drop();
		this->pavementSound = 0;
	}
}

// Function that updates source
void SoundSource::update(irrklang::vec3df position, irrklang::vec3df velMetersPerSecond, float speed, string soundString) {

	this->soundString = soundString;

	horn(this->soundString[1] == '1', position, velMetersPerSecond);
	exhaust(this->soundString[2] == '1', position);

	engineStart(this->soundString[0] == '1', position);
	terrain(this->soundString[0] == '2', speed, position, velMetersPerSecond, this->soundString[3]);
	engine(this->soundString[0] == '2', this->soundString[4], speed, position, velMetersPerSecond);
	engineOff(this->soundString[0] == '3', position);
}

// Function that plays horn sound
void SoundSource::horn(bool x, irrklang::vec3df position, irrklang::vec3df velMetersPerSec) {

	if (x) {
		// Update position and velocity
		this->hornSound->setPosition(position);
		this->hornSound->setVelocity(velMetersPerSec);

		if (this->hornSound->getIsPaused()) {
			this->hornSound->setIsPaused(false);
		}
	}
	else {
		this->hornSound->setIsPaused(true);
	}
}

// Function that plays exhaust sound
void SoundSource::exhaust(bool x, irrklang::vec3df position) {
	if (x) {
		Audio::SoundEngine->play3D("../RallySportRacing/Audio/Backfire2.wav", position);
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

// Function that plays ground sound
void SoundSource::terrain(bool x, float speed, irrklang::vec3df position, irrklang::vec3df velMetersPerSec, char terrain) {
	if (terrain == '0') {
		this->pavementSound->setPosition(position);
		this->pavementSound->setVelocity(velMetersPerSec);

		if (x) {
			if (this->pavementSound->getIsPaused()) {
				this->pavementSound->setIsPaused(false);
			}
		}
		this->pavementSound->setPlaybackSpeed(0.5F + abs(speed) / 300);
		this->pavementSound->setVolume(abs(speed) / 300);

		if (!this->dirtSound->getIsPaused()) {
			this->dirtSound->setIsPaused(true);
		}
	}
	else if (terrain == '1') {
		this->dirtSound->setPosition(position);
		this->dirtSound->setVelocity(velMetersPerSec);

		if (x) {
			if (this->dirtSound->getIsPaused()) {
				this->dirtSound->setIsPaused(false);
			}
		}
		this->dirtSound->setPlaybackSpeed(0.5F + abs(speed) / 300);
		this->dirtSound->setVolume(abs(speed) / 300);

		if (!this->pavementSound->getIsPaused()) {
			this->pavementSound->setIsPaused(true);
		}
	}
}

// Function that plays engine sound
void SoundSource::engine(bool engineOn, char WorSPressed, float speed, irrklang::vec3df position, irrklang::vec3df velMetersPerSec) {
	
	// Engine rumble
	if (engineOn) {
		//Update position and velocity
		this->engineSound->setPosition(position);
		this->engineSound->setVelocity(velMetersPerSec);

		// Play if paused
		if (this->engineSound->getIsPaused()) {
			this->engineSound->setIsPaused(false);
		}

		// Change playback speed based on speed
		this->engineSound->setPlaybackSpeed(1.0 + abs(speed) / 100);
	}
	else {
		// Pause if playing
		if (!this->engineSound->getIsPaused()) {
			this->engineSound->setIsPaused(true);
		}
	}
	
	// Engine acceleration sound
	if (engineOn) {
		this->engineHighAcc->setPosition(position);
		this->engineHighAcc->setVelocity(velMetersPerSec);

		if (WorSPressed == '1') {
			if (engineFade < 200) {
				engineFade++;
			}
		}
		else if (WorSPressed == '2') {
			if (engineFade > -200) {
				engineFade--;
			}
		}
		else {
			if (engineFade > 0) {
				engineFade--;
			}
			else if (engineFade < 0) {
				engineFade++;
			}
		}

		this->engineHighAcc->setPlaybackSpeed(1.0F + abs(engineFade) / 400.0F);
		this->engineHighAcc->setVolume(abs(engineFade) / 10.0F);
		this->engineHighAcc->setMinDistance(abs(engineFade) / 5.0F);

		if (this->engineHighAcc->getIsPaused()) {
			this->engineHighAcc->setIsPaused(false);
		}
	}
	else {
		if (!this->engineSound->getIsPaused()) {
			this->engineSound->setIsPaused(true);
		}
	}
	
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