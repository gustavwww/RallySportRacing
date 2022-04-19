#include "audio.h"
#include <iostream>
#include <string>
#include <windows.h>
#include "../../External/irrKlang/include/irrKlang.h"

#include <tuple>
#include <map>
#include <Audio/SoundSource.h>

using namespace std;
using namespace irrklang;

#pragma comment(lib, "../../External/irrKlang/irrKlang.lib")

float volume;

ISoundEngine* SoundEngine;

map<string, SoundSource*> sources;

Audio::Audio() {
	// Init sound engine
	SoundEngine = createIrrKlangDevice();

	// Init master volume
	volume = 0.3F;
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

void Audio::createSoundSource(string ID, tuple <float, float, float> position) {
	sources.insert(pair<string, SoundSource*>(ID, new SoundSource(position, SoundEngine)));
}

void Audio::updateSoundSource(string ID, tuple<float, float, float> position, float speed, bool honk) {
	sources.at(ID)->update(position, speed, honk);
}

void Audio::removeSoundSource(string ID) {
	sources.erase(ID);
}
