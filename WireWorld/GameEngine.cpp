#include "GameEngine.h"
#include "Global.h"
#include <thread>
#include <stdlib.h>

GameEngine::GameEngine() {
	init();
}
void GameEngine::init() {
	initPreSDL();
	initSDL();
	initPostSDL();
}

void GameEngine::initPreSDL() {
	totalFrames = totalUpdates = 0;
	startTime = mClock::now();

	running = true;
	screenWidth = 600;
	screenHeight = 600;
	UPS = 10;
	initRand();
}
void GameEngine::initSDL() {
	if ((SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == -1)) {
		printf("Could not initialize SDL: %s.\n", SDL_GetError());
		exit(-1);
	}
	gameWindow = SDL_CreateWindow("Wire World", 50, 50, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
	if (gameWindow == nullptr) {
		printf("Could not create window!");
	}
	//FIXME GRAPHICS SETTINGS
	//SDL_SetWindowFullscreen(gameWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
	ren = SDL_CreateRenderer(gameWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == NULL) {
		SDL_DestroyWindow(gameWindow);
		printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
		SDL_Quit();
	}
	//Initialize SDL_mixer
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
		printf("SDL2_mixer init error\n CLOSING...");
		SDL_Quit();
	}
	if (TTF_Init() == -1) {
		printf("SDL2_ttf init error\n CLOSING...");
		SDL_Quit();
	}
}

void GameEngine::initPostSDL() {
	web = new WireWeb(100, 3);
	web->startSearching();
	grid = new Grid(100, 6);
}

void GameEngine::update() {
	totalUpdates++;
	grid->update();
}
bool GameEngine::updateRequired() {
	using namespace std::chrono_literals;
	millis duration = millis(1000 / UPS) - std::chrono::duration_cast<millis>(mClock::now() - lastUpdate);
	if (duration < 1ms) {
		lastUpdate = mClock::now();
		return true;
	}
	else {
		std::this_thread::sleep_for(duration);
		return false;
	}
}

int GameEngine::updateGame(void* self) {
	GameEngine* g = (GameEngine*)self;
	while (g->running) {
		if (g->updateRequired()) {
			g->update();
		}
	}
	return 0;
}

void GameEngine::draw() {
	totalFrames++;
	SDL_RenderClear(ren);
	grid->draw(ren);
	SDL_RenderPresent(ren);
}
bool GameEngine::drawRequired() {
	return true; // don't limit fps
}
int GameEngine::renderGame(void* self) {
	GameEngine* g = (GameEngine*)self;
	while (g->running) {
		if (g->drawRequired()) {
			g->draw();
		}
	}
	return 0;
}

void GameEngine::handleEvent(SDL_Event e) {
	if (e.type == SDL_MOUSEBUTTONDOWN) {
		if (e.button.button == SDL_BUTTON_LEFT) {
			GridState gState = web->nextGrid();
			grid->setState(gState);
			grid->getState();
		}
		else if (e.button.button == SDL_BUTTON_RIGHT) {
			GridState gState = web->previousGrid();
			grid->setState(gState);
		}
	}
	else if (e.type == SDL_KEYDOWN) {
		std::cout << "Web size: " << web->size() << std::endl;
		if (e.key.keysym.sym == SDLK_q) {
			running = false;
		}
		else if (e.key.keysym.sym == SDLK_f) {
			std::cout << "fitness: (" << web->lowestFitness().first << ", " << web->averageFitness() << ", " << web->highestFitness().first << ")" << std::endl;
		}
		else if (e.key.keysym.sym == SDLK_p) {
			std::cout << "pruned: " << web->pruneWeb(web->averageFitness() * 1.33) << std::endl;
		}
		else if (e.key.keysym.sym == SDLK_c) {
			web->clear();
		}
		else if (e.key.keysym.sym == SDLK_s) {
			bool searching = web->toggleSearching();
			if (searching) {
				std::cout << "Searching..." << std::endl;
			}
			else {
				std::cout << "Stopped Searching." << std::endl;
			}
		}
		else {
			checkFPSChange(e.key.keysym.sym);
		}
	}
}
void GameEngine::checkFPSChange(SDL_Keycode k) {
	if (k == SDLK_0) {
		UPS = -1;
	}
	else if (k == SDLK_1) {
		UPS = 1;
	}
	else if (k == SDLK_2) {
		UPS = 3;
	}
	else if (k == SDLK_3) {
		UPS = 5;
	}
	else if (k == SDLK_4) {
		UPS = 10;
	}
	else if (k == SDLK_5) {
		UPS = 20;
	}
	else if (k == SDLK_6) {
		UPS = 40;
	}
	else if (k == SDLK_7) {
		UPS = 80;
	}
	else if (k == SDLK_8) {
		UPS = 160;
	}
	else if (k == SDLK_9) {
		UPS = 360;
	}
}

void GameEngine::run() {
	int updateResult, drawResult;
	updateThread = SDL_CreateThread(updateGame, "Update", this);
	drawThread = SDL_CreateThread(renderGame, "Render", this);

	SDL_Event e;
	while (running) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_WINDOWEVENT) {
				if (e.window.event == SDL_WINDOWEVENT_CLOSE) {
					running = false;
				}
			}
			handleEvent(e);
		}
	}
	std::cout << "Frames: " << totalFrames << " fps: " << (totalFrames * 1000) / std::chrono::duration_cast<millis>(mClock::now() - startTime).count() << std::endl;
	std::cout << "Updates: " << totalUpdates << std::endl;
	SDL_WaitThread(updateThread, &updateResult);
	SDL_WaitThread(drawThread, &drawResult);
	web->stopSearching();
	quit();
}

void GameEngine::quit() {
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(gameWindow);
	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}