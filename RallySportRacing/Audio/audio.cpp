#include "audio.h"

using namespace std;
using namespace irrklang;

#pragma comment(lib, "../../External/irrKlang/irrKlang.lib")

float masterVolume;

float rainFade;

ISoundEngine* SoundEngine;

map<int, SoundSource*> sources;

Audio* Audio::instance = nullptr;

// Function that ensures a single instance of audio is that which becomes available to all that need access
Audio* Audio::Instance() {
	if (instance == nullptr) {
		instance = new Audio();
	}
	return instance;
}

// Init sound engine
ISoundEngine* Audio::SoundEngine = createIrrKlangDevice();

// Constructor of audio
Audio::Audio() {

	// Declare distance scalar
	distanceScalar = 10;

	// Init master volume
	masterVolume = 1.0F;
	SoundEngine->setSoundVolume(masterVolume);

	// Init sourceless sounds
	irrklang::vec3df position(0, 0, 0);
	this->rainSound = Audio::SoundEngine->play3D("../RallySportRacing/Audio/Rain.mp3", position, true, true, true);
	this->playRain = false;
	rainFade = 0.0F;
}

// Destructor of audio
Audio::~Audio() {
	if (this->rainSound) {
		this->rainSound->stop();
		this->rainSound->drop();
		this->rainSound = 0;
	}
}

// Function that changes volume up by 5%
void Audio::volumeUp() {
	if (masterVolume < 1.0F) {
		masterVolume = masterVolume + 0.05F;
	}
	SoundEngine->setSoundVolume(masterVolume);
}

// Function that changes volume down by 5%
void Audio::volumeDown() {
	if (masterVolume > 0.0F) {
		masterVolume = masterVolume - 0.05F;
	}
	SoundEngine->setSoundVolume(masterVolume);
}

// Function that sets master volume to value v which should be a float between 0.0F and 1.0F
void Audio::volumeSet(float v) {
	masterVolume = v;
	SoundEngine->setSoundVolume(masterVolume);
}

// Function that creates sound source and adds to the map of sources with its key ID
void Audio::createSoundSource(int ID, glm::vec3 positionVec3) {
	sources.insert(pair<int, SoundSource*>(ID, new SoundSource(ID, glmToirrklangVec(positionVec3, distanceScalar))));
}

// Function that updates sound source in map with key ID
void Audio::updateSoundSource(int ID, glm::vec3 positionVec3, glm::vec3 velPerFrame, float speedKmPerh, string soundString) {
	sources.at(ID)->update(glmToirrklangVec(positionVec3, distanceScalar), getVelMetersPerSec(velPerFrame, speedKmPerh), speedKmPerh, soundString);
}

void Audio::removeSoundSource(int ID) {
	sources.erase(ID);
}

string Audio::getSoundString(int ID) {
	return sources.at(ID)->getSoundString();
}

// Function that updates the listeners position, orientation and velocity
void Audio::setListenerParameters(glm::vec3 positionVec3, glm::vec3 directionVec3, glm::vec3 velPerFrame, float speedKmPerh) {

	irrklang::vec3df position = glmToirrklangVec(positionVec3, distanceScalar);	// position of the listener
	irrklang::vec3df direction = glmToirrklangVec(directionVec3); // the direction the listener looks into
	irrklang::vec3df velMetersPerSec = getVelMetersPerSec(velPerFrame, speedKmPerh);   // only relevant for doppler effects
	irrklang::vec3df upVector(0, 1, 0);        // where 'up' is in your 3D scene

	SoundEngine->setListenerPosition(position, direction, velMetersPerSec, upVector);
}

// Function that takes a tuple of the speed per frame in X,Y,Z directions and total speed in km/h and returns a tuple of the speed in m/s
irrklang::vec3df Audio::getVelMetersPerSec(glm::vec3 velPerFrame, float speedKmPerh) {
	float x;
	// Get speed in m/s
	float speedMetersPerSec = speedKmPerh / 3.6;

	// Calculate x, the multiplier that turns the speed per frame to speed in m/s
	// speedMeterPerSec = sqrt( (velPerFrame.X * x)^2 + (velPerFrame.Y * x)^2 + (velPerFrame.Z * x)^2 ) -> x

	x = speedMetersPerSec * sqrt(pow(velPerFrame.x, 2) + pow(velPerFrame.y, 2) + pow(velPerFrame.z, 2)) /
		(pow(velPerFrame.x, 2) + pow(velPerFrame.y, 2) + pow(velPerFrame.z, 2));

	// Return vector of velocity in m/s
	irrklang::vec3df velMetersPerSec(velPerFrame.x * x, velPerFrame.y * x, velPerFrame.z * x);
	return velMetersPerSec;
}

// Function that plays audio not originating from a source
void Audio::playSourcelessSounds(glm::vec3 positionVec3) {
	rain(positionVec3);
}

// Function that plays start countdown
void Audio::playStartSound() {
	SoundEngine->play2D("../RallySportRacing/Audio/StartBeeping.wav");
}

// Function that sets rain sound to play or not
void Audio::playRainSound(bool playRain) {
	this->playRain = playRain;
}

// Function that controls rain
void Audio::rain(glm::vec3 position) {
	if (this->playRain) {
		this->rainSound->setPosition(glmToirrklangVec(position, distanceScalar));

		if (rainFade < 0.2F) {
			rainFade += 0.004F;

			this->rainSound->setVolume(rainFade);

			if (this->rainSound->getIsPaused()) {
				this->rainSound->setIsPaused(false);
			}
		}
	}
	else if (rainFade > 0.0F) {
		rainFade -= 0.004F;
		this->rainSound->setPosition(glmToirrklangVec(position, distanceScalar));

		this->rainSound->setVolume(rainFade);
	}
	else if (rainFade == 0.0F) {
		this->rainSound->setIsPaused(true);
	}
}

// Function that plays sound when button is pressed
void Audio::playButtonPressSound() {
	SoundEngine->play2D("../RallySportRacing/Audio/ButtonPress.wav");
}

// Function that plays checkpoint reached sound
void Audio::playCheckpointSound() {
	SoundEngine->play2D("../RallySportRacing/Audio/StartBeep1.mp3");
}

// Function that plays finish line reached sound
void Audio::playFinishSound() {
	SoundEngine->play2D("../RallySportRacing/Audio/StartBeep2.mp3");
}

// Function that converts glm::vec3 to irrklang::vec3df
irrklang::vec3df Audio::glmToirrklangVec(glm::vec3 inVec) {
	irrklang::vec3df outVec(inVec.x, inVec.y, inVec.z);
	return outVec;
}
irrklang::vec3df Audio::glmToirrklangVec(glm::vec3 inVec, int scalar) {
	irrklang::vec3df outVec(inVec.x / scalar, inVec.y / scalar, inVec.z / scalar);
	return outVec;
}