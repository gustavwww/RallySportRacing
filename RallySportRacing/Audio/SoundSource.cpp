#include "SoundSource.h"


using namespace std;
using namespace irrklang;

#pragma comment(lib, "../../External/irrKlang/irrKlang.lib")

float speed;

int engineFade;
float revv;
float revvPitch;
float revvMult;

bool checkStart;

SoundSource::SoundSource(int ID, irrklang::vec3df position) {
	// Init engine sound
	speed = 0.0F;
	revv = 1.0F;
	revvPitch = 0.0F;
	revvMult = 1.0F;
	checkStart = false;

	this->soundString = "00000";
	startSoundTimer = 0;
	
	// Init looping sounds
	this->hornSound = Audio::SoundEngine->play3D("../RallySportRacing/Audio/ES_Horn Honk Long - SFX Producer.wav", position, true, true, true);


	this->engineSound = Audio::SoundEngine->play3D("../RallySportRacing/Audio/rumble.wav",position,  true, true, true);
	this->engineHighAcc = Audio::SoundEngine->play3D("../RallySportRacing/Audio/acceleration.wav", position, true, true, true);
	this->engineHighDec = Audio::SoundEngine->play3D("../RallySportRacing/Audio/decceleration.wav", position, true, true, true);

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
	if (this->engineHighAcc) {
		this->engineHighAcc->stop();
		this->engineHighAcc->drop();
		this->engineHighAcc = 0;
	}
	if (this->engineHighDec) {
		this->engineHighDec->stop();
		this->engineHighDec->drop();
		this->engineHighDec = 0;
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
		this->engineSound->setPlaybackSpeed(1.0 + abs(speed) / 400);
	}
	else {
		// Pause if playing
		if (!this->engineSound->getIsPaused()) {
			this->engineSound->setIsPaused(true);
		}
	}
	
	// Engine acceleration sound
	if (engineOn) {

		// If acceleration pressed
		if (WorSPressed == '1') {
			if (engineFade < 0) {
				engineFade = 0;
			}
			if (engineFade < 100) {
				engineFade++;
			}

			// If accelerating turn off decceleration sound
			if (!this->engineHighDec->getIsPaused()) {
				this->engineHighDec->setIsPaused(true);
			}

			// If accelerating turn acceleration sound on and set parameters
			this->engineHighAcc->setPosition(position);
			this->engineHighAcc->setVelocity(velMetersPerSec);

			if (this->engineHighAcc->getIsPaused()) {
				this->engineHighAcc->setIsPaused(false);
			}
			
			if (speed < 10) {
				checkStart = true;
			}
			// Acceleration sound at high speeds
			if((5 * speed / 400.0F) > (engineFade / 200.0F) ) {
				this->engineHighAcc->setPlaybackSpeed(1.0F + abs(speed) / 400.0F);
				this->engineHighAcc->setVolume((abs(speed) / 100.0F) * engineFade / 100.0F);

				checkStart = false;
				revv = 1.0F;
			}
			// Acceleration sound at low speeds
			else if (checkStart) {
				
				// Revv functionality
				if (engineFade > 90) {
					if (revvPitch < 2.0F) {
						revvPitch += 0.3F;
					}
					if (revv > 1.8F) {
						revv = 0.5F;
						//revvMult = 1.0F;
					}
					else {
						//revv = revvMult * revv;
						//revvMult += 0.01;
						revv += 0.2;
					}
				}

				// Acceleration sound set states
				this->engineHighAcc->setPlaybackSpeed( (1.0F + engineFade / 200.0F) * revvPitch);
				this->engineHighAcc->setVolume( (engineFade / 100.0F) * revv);
			}
		}

		// If decceleration pressed
		else if (WorSPressed == '2') {
			
			if (engineFade < 0) {
				engineFade = 0;
			}
			if (engineFade > 0) {
				engineFade-=5;
			}

			// If deccelerating turn acceleration sound off
			if (!this->engineHighAcc->getIsPaused()) {
				this->engineHighAcc->setIsPaused(true);
			}

			// If deccelerating turn decceleration sound on and set parameters
			this->engineHighDec->setPosition(position);
			this->engineHighDec->setVelocity(velMetersPerSec);

			if (this->engineHighDec->getIsPaused()) {
				this->engineHighDec->setIsPaused(false);
			}

			//
			this->engineHighDec->setPlaybackSpeed(1.0F + abs(speed) / 400.0F);
			this->engineHighDec->setVolume((abs(speed) / 200.0F));
		}
		// If neither activly accelerating or decellerating
		else {
			if (engineFade > 0) {
				engineFade--;
			}
			else if (engineFade < 0) {
				engineFade++;
			}

			// If neither turn acceleration sound off
			if (!this->engineHighAcc->getIsPaused()) {
				this->engineHighAcc->setIsPaused(true);
			}

			// If neither turn decelleration sound on
			this->engineHighDec->setPosition(position);
			this->engineHighDec->setVelocity(velMetersPerSec);

			if (this->engineHighDec->getIsPaused()) {
				this->engineHighDec->setIsPaused(false);
			}

			//
			this->engineHighDec->setPlaybackSpeed(1.0F + abs(speed) / 400.0F);
			this->engineHighDec->setVolume((abs(speed) / 200.0F));
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