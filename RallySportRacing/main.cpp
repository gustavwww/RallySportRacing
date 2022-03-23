#include "Rendering/SDLWindowHandler.h"
#include "Rendering/Model.h"
#include "Game/GameManager.h"
#include "Services/Protocol/ProtocolParser.h"
#include "Services/Protocol/Command.h"
#include "Services/TCPClient.h"
#include <thread>

const int WIDTH = 1920, HEIGHT = 1080;

using namespace Rendering;

int main(int argc, char* argv[]) {

	SDLWindowHandler windowHandler(WIDTH, HEIGHT);
	windowHandler.initSDLWindow("OpenGL");

	Game::setupGame(&windowHandler);

	windowHandler.beginRenderingLoop(Game::update);

	windowHandler.Destroy();

	return 0;
}