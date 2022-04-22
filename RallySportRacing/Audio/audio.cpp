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
irrklang::ISoundSource* Audio::hornSound = SoundEngine->addSoundSourceFromFile("../RallySportRacing/Audio/ES_Horn Honk Long - SFX Producer.mp3");
irrklang::ISoundSource* Audio::exhaustSound = SoundEngine->addSoundSourceFromFile("../RallySportRacing/Audio/Backfire.mp3");
irrklang::ISoundSource* Audio::engineStartSound = SoundEngine->addSoundSourceFromFile("../RallySportRacing/Audio/engineStart2.mp3");
irrklang::ISoundSource* Audio::engineOffSound = SoundEngine->addSoundSourceFromFile("../RallySportRacing/Audio/engineOff.mp3");

irrklang::ISound* Audio::engineSound = SoundEngine->play2D("../RallySportRacing/Audio/BetterCarAudio.mp3", true, true, true);

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
	sources.insert(pair<int, SoundSource*>(ID, new SoundSource(position)));
	cout << "Creted sound source with ID: " + to_string(ID) << endl;
}

void Audio::updateSoundSource(int ID, tuple<float, float, float> position, float speed, string sounds) {
	sources.at(ID)->update(position, speed, sounds);
	/*if (ID != 0) {
		cout << "Updated sound source with ID: " + to_string(ID) << endl;
	}*/
}

void Audio::removeSoundSource(int ID) {
	sources.erase(ID);
	cout << "Removed sound source: " + to_string(ID) << endl;
}

string Audio::getSoundSourceSounds(int ID) {
	return sources.at(ID)->getSounds();
}
