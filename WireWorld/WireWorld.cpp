#include "GameEngine.h"

int main(int argc, char** argv) {
	GameEngine* g = new GameEngine();
	g->run();
	std::cin.get();
	return 0;
}