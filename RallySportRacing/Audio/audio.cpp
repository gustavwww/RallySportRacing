#include "audio.h"

using namespace std;
using namespace irrklang;

#pragma comment(lib, "../../External/irrKlang/irrKlang.lib")

float volume;

ISoundEngine* SoundEngine;

int Audio::distanceScalar = 10;

map<int, SoundSource*> sources;

Audio* Audio::instance = nullptr;

Audio* Audio::Instance() {
	if (instance == nullptr) {
		instance = new Audio();
	}
	return instance;
}

// Init sound engine
ISoundEngine* Audio::SoundEngine = createIrrKlangDevice();

Audio::Audio() {

	// Init master volume
	volume = 1.0F;
	SoundEngine->setSoundVolume(volume);
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

void Audio::createSoundSource(int ID, glm::vec3 position) {
	sources.insert(pair<int, SoundSource*>(ID, new SoundSource(ID, position)));
}

// Function that updates sound source in map with key ID
void Audio::updateSoundSource(int ID, glm::vec3 position, glm::vec3 velPerFrame, float speed, string sounds) {
	sources.at(ID)->update(position, velPerFrame, speed, sounds);
}

void Audio::removeSoundSource(int ID) {
	sources.erase(ID);
}

string Audio::getSoundString(int ID) {
	return sources.at(ID)->getSoundString();
}

// Function that updates the listeners position, orientation and velocity
void Audio::setListenerParameters(glm::vec3 positionXYZ, glm::vec3 direction, glm::vec3 velPerFrame, float speedKmPerh) {

	irrklang::vec3df position(positionXYZ.x/distanceScalar, positionXYZ.y/distanceScalar, positionXYZ.z/distanceScalar);	// position of the listener
	irrklang::vec3df lookDirection(direction.x, direction.y, direction.z); // the direction the listener looks into
	irrklang::vec3df velPerSecond = getVelMetersPerSec(velPerFrame, speedKmPerh);   // only relevant for doppler effects
	irrklang::vec3df upVector(0, 1, 0);        // where 'up' is in your 3D scene

	SoundEngine->setListenerPosition(position, lookDirection, velPerSecond, upVector);
}

// Function that takes a tuple of the speed per frame in X,Y,Z directions and total speed in km/h and returns a tuple of the speed in m/s
irrklang::vec3df Audio::getVelMetersPerSec(glm::vec3 velPerFrame, float speedKmPerh) {
	float x;
	// Get speed in m/s
	float speedMetersPerSec = speedKmPerh / 3.6;
	
	// Calculate x, the multiplier that turns the speed per frame to speed in m/s
	// speedMeterPerSec = sqrt( (velPerFrame.X * x)^2 + (velPerFrame.Y * x)^2 + (velPerFrame.Z * x)^2 ) -> x

	x = speedMetersPerSec * sqrt( pow(velPerFrame.x, 2) + pow(velPerFrame.y, 2) + pow(velPerFrame.z, 2) ) /
		(pow(velPerFrame.x, 2) + pow(velPerFrame.y, 2) + pow(velPerFrame.z, 2));

	// Return vector of velocity in m/s
	irrklang::vec3df velMetersPerSec( velPerFrame.x * x, velPerFrame.y * x,velPerFrame.z * x );
	return velMetersPerSec;
}


