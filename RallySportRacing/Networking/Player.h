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

using namespace std;

namespace Networking {

	class Player {
	public:
		Player(Rendering::SDLWindowHandler* windowHandler, Networking::PlayerData playerData);
		~Player();
		void updateState(PlayerData data);
		string getName();
		Rendering::Model* getModel();

	private:
		string name;
		Game::GameObject* obj;
		Game::GameObject* frontLeft;
		Game::GameObject* frontRight;
		Game::GameObject* backLeft;
		Game::GameObject* backRight;
	};

}



#endif