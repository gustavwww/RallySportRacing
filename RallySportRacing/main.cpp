#include "Rendering/SDLWindowHandler.h"
#include "Rendering/Model.h"
#include "Game/GameManager.h"

const int WIDTH = 800, HEIGHT = 600;

using namespace Rendering;

int main(int argc, char* argv[]) {

	SDLWindowHandler windowHandler(WIDTH, HEIGHT);
	windowHandler.initSDLWindow("OpenGL");

	Game::setupGame(&windowHandler);

	windowHandler.beginRenderingLoop(Game::update);

	windowHandler.Destroy();

	return 0;
}