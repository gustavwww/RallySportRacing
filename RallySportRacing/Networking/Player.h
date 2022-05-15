#ifndef PLAYER
#define PLAYER

#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Game/GameObject.h"
#include "Rendering/Model.h"
#include "Rendering/SDLWindowHandler.h"
#include "Services/Protocol/Command.h"
#include "Networking/PlayerData.h"
#include "Rendering/Text2D.h"

using namespace std;

namespace Networking {

	class Player {
	public:
		Player(Rendering::SDLWindowHandler* windowHandler, string name, int color);
		~Player();
		void updateState(PlayerData data);
		void subUpdate(float deltaTime);
		void setName(string name);
		void updateColor();
		void setColor(int color);
		string getName();
		int getColor();

	private:
		bool isInitialized = false;
		void initPlayer(PlayerData data);

		Rendering::SDLWindowHandler* windowHandler;
		string name;
		int color;

		glm::vec3 velocity;
		Rendering::Text2D* label;
		Game::GameObject* obj;
		Game::GameObject* frontLeft;
		Game::GameObject* frontRight;
		Game::GameObject* backLeft;
		Game::GameObject* backRight;
	};

}



#endif