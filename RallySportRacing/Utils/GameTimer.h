#ifndef TIMER
#define TIMER
#include <chrono>

using namespace std;

namespace Utils {

	class GameTimer {
	public:
		static GameTimer* Instance();
		static void Release();

		void startGameTime();
		void updateGameTime();
		float getDeltaTime();

	private:
		static GameTimer* instance;
		chrono::steady_clock::time_point previousTime;
		chrono::steady_clock::time_point currentTime;
		float deltaTime;

		GameTimer();
		~GameTimer();
	};
}
#endif