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

// Function that creates one and only one instance of Audio
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

	// Init master volume
	volume = 0.3F;
	SoundEngine->setSoundVolume(volume);
}

// Function that increases master volume by 5%
void Audio::volumeUp() {
	if (volume < 1.0F) {
		volume = volume + 0.05F;
	}
	SoundEngine->setSoundVolume(volume);
}

// Function that decreases master volume by 5%
void Audio::volumeDown() {
	if (volume > 0.0F) {
		volume = volume - 0.05F;
	}
	SoundEngine->setSoundVolume(volume);
}

// Function that sets master volume to v
void Audio::volumeSet(float v) {
	volume = v;
	SoundEngine->setSoundVolume(volume);
}

// Function that creates a sound source and ads ut to a map of sources
void Audio::createSoundSource(int ID, tuple <float, float, float> position) {
	sources.insert(pair<int, SoundSource*>(ID, new SoundSource(ID, position)));
}

// Function that updates sound source in map with key ID
void Audio::updateSoundSource(int ID, tuple<float, float, float> position, float speed, string sounds) {
	sources.at(ID)->update(position, speed, sounds);
}

// Function that removes sound source from map and ends all sounds
void Audio::removeSoundSource(int ID) {
	sources.at(ID)->removeSoundSource();
	sources.erase(ID);
}

// Function that gets sound string of specific sound source
string Audio::getSoundString(int ID) {
	return sources.at(ID)->getSoundString();
}
