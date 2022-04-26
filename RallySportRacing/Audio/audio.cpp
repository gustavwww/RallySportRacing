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

// Init various sounds
//irrklang::ISoundSource* Audio::hornSound = SoundEngine->addSoundSourceFromFile("../RallySportRacing/Audio/ES_Horn Honk Long - SFX Producer.mp3");
//irrklang::ISoundSource* Audio::exhaustSound = SoundEngine->addSoundSourceFromFile("../RallySportRacing/Audio/Backfire.mp3");
//irrklang::ISoundSource* Audio::engineStartSound = SoundEngine->addSoundSourceFromFile("../RallySportRacing/Audio/engineStart2.mp3");
//irrklang::ISoundSource* Audio::engineOffSound = SoundEngine->addSoundSourceFromFile("../RallySportRacing/Audio/engineOff.mp3");

//irrklang::ISound* Audio::engineSound = SoundEngine->play2D("../RallySportRacing/Audio/BetterCarAudio.mp3", true, true, true);

Audio::Audio() {

	// Init master volume
	volume = 0.3F;
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
	cout << "Creted sound source with ID: " + to_string(ID) << endl;
}

// Function that updates sound source in map with key ID
void Audio::updateSoundSource(int ID, tuple<float, float, float> position, tuple<float, float, float> velPerFrame, float speed, string sounds) {
	sources.at(ID)->update(position, velPerFrame, speed, sounds);
}

void Audio::removeSoundSource(int ID) {
	sources.erase(ID);
	cout << "Removed sound source: " + to_string(ID) << endl;
}

string Audio::getSoundString(int ID) {
	return sources.at(ID)->getSoundString();
}

// Function that updates the listeners position, orientation and velocity
void Audio::setListenerParameters(tuple <float, float, float> positionXYZ, tuple <float, float, float> direction, tuple <float, float, float> velPerFrame, float speedKmPerh) {

	irrklang::vec3df position(get<0>(positionXYZ), get<1>(positionXYZ), get<2>(positionXYZ));	// position of the listener
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
