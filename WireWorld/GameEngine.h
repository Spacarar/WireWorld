#pragma once
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include "Grid.h"
#include "WireWeb.h"
#include "Global.h"

class GameEngine {
	int UPS;
	size_t totalFrames;
	size_t totalUpdates;
	
	time_point startTime;

	int screenWidth;
	int screenHeight;

	bool running;

	WireWeb* web; //searching
	Grid* grid; //display

	SDL_Thread* updateThread, * drawThread;
	SDL_Window* gameWindow;
	SDL_Renderer* ren;

	time_point lastUpdate, lastDraw;
	millis currentTime;

	void init();
	void initPreSDL();
	void initSDL();
	void initPostSDL();

	static int updateGame(void* self);
	bool updateRequired();
	static int renderGame(void* self);
	bool drawRequired();
	
	void update();
	void draw();

	void handleEvent(SDL_Event e);
	void checkFPSChange(SDL_Keycode k);

public:
	GameEngine();
	void run();
	void quit();
};