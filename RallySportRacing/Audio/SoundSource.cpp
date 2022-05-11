#include "SoundSource.h"


using namespace std;
using namespace irrklang;

#pragma comment(lib, "../../External/irrKlang/irrKlang.lib")

float speed;

float rainFade;

int wPressed;
int sPressed;

SoundSource::SoundSource(int ID, glm::vec3 positionVec3)
{
	// Set inital position of sound source
	irrklang::vec3df position(positionVec3.x/Audio::distanceScalar, positionVec3.y/ Audio::distanceScalar, positionVec3.z/ Audio::distanceScalar);
	speed = 0.0F;

	// Init engine sound

	this->soundString = "00000";
	startSoundTimer = 0;

	playRain = false;
	rainFade = 0;
	
	// Init looping sounds
	this->hornSound = Audio::SoundEngine->play3D("../RallySportRacing/Audio/ES_Horn Honk Long - SFX Producer.mp3", position, true, true, true);


	this->engineSound = Audio::SoundEngine->play3D("../RallySportRacing/Audio/EngineRumble.mp3",position,  true, true, true);
	this->engineSoundHigh = Audio::SoundEngine->play3D("../RallySportRacing/Audio/EngineRumble.mp3", position, true, true, true);


	this->rainSound = Audio::SoundEngine->play3D("../RallySportRacing/Audio/Rain.mp3", position, true, true, true);
	this->pavementSound = Audio::SoundEngine->play3D("../RallySportRacing/Audio/TerrainPavement2.mp3", position, true, true, true);
	this->dirtSound = Audio::SoundEngine->play3D("../RallySportRacing/Audio/TerrainDirt.mp3", position, true, true, true);
}

// Destructor of SoundSource
SoundSource::~SoundSource()	{
	if (this->hornSound) {
		this->hornSound->stop();
		this->hornSound->drop();
		this->hornSound = 0;
	}
	if (this->rainSound) {
		this->rainSound->stop();
		this->rainSound->drop();
		this->rainSound = 0;
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
void SoundSource::update(glm::vec3 positionVec3, glm::vec3 velPerFrame, float speed, string soundString) {
	irrklang::vec3df position(positionVec3.x/Audio::distanceScalar, positionVec3.y/Audio::distanceScalar, positionVec3.z/Audio::distanceScalar);

	// Get velocity vector in m/s
	irrklang::vec3df velMetersPerSecond = Audio::getVelMetersPerSec(velPerFrame, speed);

	this->soundString = soundString;

	horn(this->soundString[1] == '1', position, velMetersPerSecond);
	exhaust(this->soundString[2] == '1', position);
	rain(playRain, position);

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

// Function that controls rain
void SoundSource::rain(bool x, irrklang::vec3df position) {
	if(x) {
		this->rainSound->setPosition(position);

		if (rainFade < 1.0F) {
			rainFade += 0.001F;

			this->rainSound->setVolume(rainFade);

			if (this->rainSound->getIsPaused()) {
				this->rainSound->setIsPaused(false);
			}
		}
	}
	else if (rainFade > 0.0F) {
		rainFade -= 0.001F;
		this->rainSound->setPosition(position);

		this->rainSound->setVolume(rainFade);
	}
	else if (rainFade == 0.0F) {
		this->rainSound->setIsPaused(true);
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

	if (engineOn) {
		//Update position and velocity
		this->engineSound->setPosition(position);
		this->engineSound->setVelocity(velMetersPerSec);

		this->engineSoundHigh->setPosition(position);
		this->engineSoundHigh->setVelocity(velMetersPerSec);

		// Play if paused
		if (this->engineSound->getIsPaused()) {
			this->engineSound->setIsPaused(false);
		}
	}
	else {
		// Pause if playing
		if (!this->engineSound->getIsPaused()) {
			this->engineSound->setIsPaused(true);
		}
	}
	// Change playback speed based on speed
	this->engineSound->setPlaybackSpeed(1.0 + abs(speed) / 100);


	if (engineOn && WorSPressed == '1') {
		if (wPressed < 1000) {
			wPressed++;
		}

		this->engineSoundHigh->setPlaybackSpeed(2.0F + abs(wPressed) / 1000);
		this->engineSoundHigh->setVolume(0.5F + abs(wPressed) / 1000);

		if (this->engineSoundHigh->getIsPaused()) {
			this->engineSoundHigh->setIsPaused(false);
		}
	}
	else {
		if (!this->engineSoundHigh->getIsPaused()) {
			this->engineSoundHigh->setIsPaused(true);
		}

		if (wPressed > 0) {
			wPressed--;
		}
	}

}

// Function that plays engine off sound
void SoundSource::engineOff(bool x, irrklang::vec3df position) {
	if (x) {
		Audio::SoundEngine->play3D("../RallySportRacing/Audio/engineOff.mp3", position);
	}
}

void SoundSource::setPlayRain(bool playRain) {
	this->playRain = playRain;
}

string SoundSource::getSoundString() {
	return this->soundString;
}