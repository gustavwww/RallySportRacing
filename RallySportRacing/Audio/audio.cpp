#include "audio.h"
#include <iostream>
#include <string>
#include <windows.h>
#include "../../External/irrKlang/include/irrKlang.h"

#include <tuple>
#include <map>
#include "SoundSource.h"

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

void Audio::createSoundSource(int ID, tuple <float, float, float> position) {
	sources.insert(pair<int, SoundSource*>(ID, new SoundSource(ID, position)));
}

// Function that updates sound source in map with key ID
void Audio::updateSoundSource(int ID, tuple<float, float, float> position, tuple<float, float, float> velPerFrame, float speed, string sounds) {
	sources.at(ID)->update(position, velPerFrame, speed, sounds);
}

void Audio::removeSoundSource(int ID) {
	sources.erase(ID);
}

string Audio::getSoundString(int ID) {
	return sources.at(ID)->getSoundString();
}

// Function that updates the listeners position, orientation and velocity
void Audio::setListenerParameters(tuple <float, float, float> positionXYZ, tuple <float, float, float> direction, tuple <float, float, float> velPerFrame, float speedKmPerh) {

	irrklang::vec3df position(get<0>(positionXYZ)/distanceScalar, get<1>(positionXYZ)/distanceScalar, get<2>(positionXYZ)/distanceScalar);	// position of the listener
	irrklang::vec3df lookDirection(get<0>(direction), get<1>(direction), get<2>(direction)); // the direction the listener looks into
	irrklang::vec3df velPerSecond = getVelMetersPerSec(velPerFrame, speedKmPerh);   // only relevant for doppler effects
	irrklang::vec3df upVector(0, 1, 0);        // where 'up' is in your 3D scene

	SoundEngine->setListenerPosition(position, lookDirection, velPerSecond, upVector);
}

// Function that takes a tuple of the speed per frame in X,Y,Z directions and total speed in km/h and returns a tuple of the speed in m/s
irrklang::vec3df Audio::getVelMetersPerSec(tuple <float, float, float> velPerFrame, float speedKmPerh) {
	float x;
	// Get speed in m/s
	float speedMetersPerSec = speedKmPerh / 3.6;
	
	// Calculate x, the multiplier that turns the speed per frame to speed in m/s
	// speedMeterPerSec = sqrt( (velPerFrame.X * x)^2 + (velPerFrame.Y * x)^2 + (velPerFrame.Z * x)^2 ) -> x

	x = speedMetersPerSec * sqrt( pow(get<0>(velPerFrame), 2) + pow(get<1>(velPerFrame), 2) + pow(get<2>(velPerFrame), 2) ) /
		(pow(get<0>(velPerFrame), 2) + pow(get<1>(velPerFrame), 2) + pow(get<2>(velPerFrame), 2));

	// Return vector of velocity in m/s
	irrklang::vec3df velMetersPerSec( get<0>(velPerFrame) * x, get<1>(velPerFrame) * x, get<2>(velPerFrame) * x );
	return velMetersPerSec;
}

// Return speed and velocity in string form
string Audio::velocityToString(float speedKmPerh, tuple<float, float, float> velPerFrame)
{
	return to_string(speedKmPerh) + "|" + to_string(get<0>(velPerFrame)) + "|" + to_string(get<1>(velPerFrame)) + "|" + to_string(get<2>(velPerFrame)) + "|";
}

// Return speed and velocity in tuple form
tuple<float, float, float, float> Audio::stringToVelocity(string velocity)
{
	int arrayIterator = 0;
	string tempFloatArray[4];

	for (int i = 0; i < velocity.length(); i++) {
		if (velocity[i] != '|') {
			tempFloatArray[arrayIterator].append(&velocity[i]);
		}
		else {
			arrayIterator++;
		}
	}
	return make_tuple(stof(tempFloatArray[0]), stof(tempFloatArray[1]), stof(tempFloatArray[2]), stof(tempFloatArray[3]));
}
