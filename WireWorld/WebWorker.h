#pragma once
#include <map>
#include <SDL.h>
#include "Grid.h"
#include "GridState.h"
#include "LifeRecord.h"

enum class WorkCycle {WORKLIVING, REPEATED, GLOBALREPEATED, DONE, WORKFAILED};

class WebWorker {
	SDL_mutex* workMutex;
	Grid* grid;
	unsigned int gridSize;
	SDL_Thread* workerThread;

	std::map<size_t, LifeRecord>* personalMap;
	std::map<size_t, LifeRecord>* globalMap;

	bool searching;

	static int startSearching(void* self);

	GridState selectStartingGrid();
	GridState pickRandom(unsigned int pieces);
	GridState pickUnion();
	// TODO more picking types

	WorkCycle updateGrid();
	WorkCycle dumpData();

	void clear();

public:
	WebWorker();
	WebWorker(unsigned int size, std::map<size_t, LifeRecord>* global, SDL_mutex* mute);
	~WebWorker();

	void start();
	int stop();
};