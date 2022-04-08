#include "GameTimer.h"

namespace Utils {
	
	//Singleton
	GameTimer* GameTimer::instance = nullptr;

	GameTimer* GameTimer::Instance() {
		if (instance == nullptr) {
			instance = new GameTimer();
		}

		return instance;
	}

	void GameTimer::Release() {
		delete instance;
		instance = nullptr;
	}

	void GameTimer::startGameTime() {
		currentTime = chrono::high_resolution_clock::now();
	}

	void GameTimer::updateGameTime(){
		previousTime = currentTime;
		currentTime = chrono::high_resolution_clock::now();
		deltaTime = chrono::duration<float, milli>(currentTime - previousTime).count() * 0.001;
	}

	float GameTimer::getDeltaTime() {
		return deltaTime;
	}

	GameTimer::GameTimer() {
		
	}

	GameTimer::~GameTimer() {
		Release();
	}

}