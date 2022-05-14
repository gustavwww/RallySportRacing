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
		Player(Rendering::SDLWindowHandler* windowHandler, Networking::PlayerData playerData);
		~Player();
		void updateState(PlayerData data);
		void subUpdate(float deltaTime);
		string getName();

	private:
		Rendering::SDLWindowHandler* windowHandler;
		string name;
		string color;
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