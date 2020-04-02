#pragma once
#include <map>
#include <SDL.h>
#include "Grid.h"
#include "GridState.h"
#include "LifeRecord.h"
#include "WebWorker.h"

class WireWeb {
	std::map<size_t, LifeRecord> web;
	unsigned int shownIndex;
	bool searching;
	unsigned int numWorkers;
	WebWorker** workers;
	SDL_mutex* workMutex;

public:
	WireWeb(unsigned int gridSize = 4, unsigned int workerCount = 1);
	~WireWeb();

	size_t size();

	bool isSearching();
	void startSearching();
	void stopSearching();
	bool toggleSearching(); //return search state

	size_t pruneWeb(long belowFitness = 0); // return number of removed elements
	std::pair<long, size_t> lowestFitness();
	std::pair<long, size_t> highestFitness();
	long averageFitness();

	void clear();

	GridState nextGrid();
	GridState previousGrid();
};