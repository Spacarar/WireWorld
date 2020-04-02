#include "WireWeb.h"
#include <iostream>

WireWeb::WireWeb(unsigned int gridSize, unsigned int workerCount) {
	web.clear();
	shownIndex = 0;
	searching = false;
	numWorkers = workerCount;
	workers = new WebWorker * [numWorkers];
	workMutex = SDL_CreateMutex();
	for (unsigned int i = 0; i < workerCount; i++) {
		workers[i] = new WebWorker(gridSize, &web, workMutex);
	}
}
WireWeb::~WireWeb() {
	for (unsigned int i = 0; i < numWorkers; i++) {
		delete workers[i];
	}
	delete[] workers;
	SDL_DestroyMutex(workMutex);
}

size_t WireWeb::size() {
	return web.size();
}

bool WireWeb::isSearching() {
	return searching;
}

void WireWeb::startSearching() {
	searching = true;
	for (unsigned int i = 0; i < numWorkers; i++) {
		workers[i]->start();
	}
}
void WireWeb::stopSearching() {
	searching = false;
	for (unsigned int i = 0; i < numWorkers; i++) {
		workers[i]->stop();
	}
}
bool WireWeb::toggleSearching() {
	if (searching) {
		searching = false;
		for (unsigned int i = 0; i < numWorkers; i++) {
			workers[i]->stop();
		}
	}
	else {
		searching = true;
		for (unsigned int i = 0; i < numWorkers; i++) {
			workers[i]->start();
		}
	}
	return searching;
}

size_t WireWeb::pruneWeb(long belowFitness) {
	// FIXME all related records should also be removed.
	// probably could be done with a queue or stack
	std::map<size_t, LifeRecord>::iterator it;
	size_t removed = 0;
	if (SDL_LockMutex(workMutex) == 0) {
		for (it = web.begin(); it != web.end(); /*iterate within loop*/) {
			if (it->second.fitness < belowFitness) {
				web.erase(it++);
				removed += 1;
			}
			else {
				++it;
			}
		}
		SDL_UnlockMutex(workMutex);
	}

	return removed;
}
std::pair<long, size_t> WireWeb::lowestFitness() {
	bool unset = true;
	std::pair<long, size_t> temp = std::make_pair(0, 0);
	if (SDL_LockMutex(workMutex) == 0) {
		std::map<size_t, LifeRecord>::iterator it;
		for (it = web.begin(); it != web.end(); it++) {
			if (it->second.fitness > temp.first || unset) {
				temp = std::make_pair(it->second.fitness, it->first);
				unset = false;
			}
		}
		SDL_UnlockMutex(workMutex);
	}
	return temp;
}
std::pair<long, size_t> WireWeb::highestFitness() {
	bool unset = true;
	std::pair<long, size_t> temp = std::make_pair(0, 0);
	if (SDL_LockMutex(workMutex) == 0) {
		std::map<size_t, LifeRecord>::iterator it;
		for (it = web.begin(); it != web.end(); it++) {
			if (it->second.fitness < temp.first || unset) {
				temp = std::make_pair(it->second.fitness, it->first);
				unset = false;
			}
		}
		SDL_UnlockMutex(workMutex);
	}
	return temp;
}
long WireWeb::averageFitness() {
	long temp = 0;
	if (!web.size()) {
		return temp;
	}
	if (SDL_LockMutex(workMutex) == 0) {
		std::map<size_t, LifeRecord>::iterator it;
		for (it = web.begin(); it != web.end(); it++) {
			temp += it->second.fitness;
		}
		temp = floor(temp / web.size());
		SDL_UnlockMutex(workMutex);
	}
	return temp;
}

void WireWeb::clear() {
	if (SDL_LockMutex(workMutex) == 0) {
		web.clear();
		SDL_UnlockMutex(workMutex);
	}
}

GridState WireWeb::nextGrid() {
	if (!web.size()) { // no div / 0
		GridState temp;
		return temp;
	}
	shownIndex = (shownIndex + 1) % web.size();
	std::map<size_t, LifeRecord>::iterator it = web.begin();
	std::advance(it, shownIndex);
	std::cout << "Next Grid: f(" << it->second.fitness << ") c(" << it->second.state.conduits().size() << ") h(" << it->second.state.heads().size() << ") t(" << it->second.state.tails().size() << ")" << std::endl;
	return it->second.state;
}

GridState WireWeb::previousGrid() {
	if (!web.size()) {
		GridState temp;
		return temp;
	}
	shownIndex = (shownIndex - 1) % web.size();
	std::map<size_t, LifeRecord>::iterator it = web.begin();
	std::advance(it, shownIndex);
	return it->second.state;
}