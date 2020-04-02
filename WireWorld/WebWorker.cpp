#include "WebWorker.h"
#include <stdlib.h>
#include <iostream>

WebWorker::WebWorker() {
	this->grid = new Grid();
	this->gridSize = 0;
	this->workerThread = nullptr;
	this->searching = false;
	this->personalMap = nullptr;
	this->globalMap = nullptr;
	this->workMutex = nullptr;
}

WebWorker::WebWorker(unsigned int size, std::map<size_t, LifeRecord>* global, SDL_mutex* mute) {
	this->gridSize = size;
	this->grid = new Grid(size);
	this->globalMap = global;
	this->personalMap = new std::map<size_t, LifeRecord>;
	this->workerThread = nullptr;
	this->searching = false;
	this->workMutex = mute;
}

WebWorker::~WebWorker() {
	if (this->personalMap != nullptr) {
		this->personalMap->clear();
		delete this->personalMap;
	}
}

void WebWorker::start() {
	this->searching = true;
	this->workerThread = SDL_CreateThread(this->startSearching, "worker searching", this);
}

int WebWorker::startSearching(void* self) {
	WebWorker* worker = (WebWorker*)self;
	if (worker->globalMap == nullptr || worker->workMutex == nullptr) {
		return -1;
	}

	WorkCycle cycle = WorkCycle::WORKLIVING;
	while (worker->searching) {
		worker->clear();
		GridState initialGrid = worker->selectStartingGrid();
		worker->grid->setState(initialGrid);

		cycle = WorkCycle::WORKLIVING;
		while (cycle == WorkCycle::WORKLIVING && worker->searching) {
			cycle = worker->updateGrid();
		}
		if (cycle == WorkCycle::REPEATED || cycle == WorkCycle::GLOBALREPEATED) {
			cycle = worker->dumpData();
		}
	}
	return 0;
}

GridState WebWorker::selectStartingGrid() {
	int choice = rand() % 2;
	if (choice == 1) {
		return pickUnion();
	}
	// choice 0 or fallback return
	return pickRandom(rand() % gridSize * gridSize / 2);
}

GridState WebWorker::pickRandom(unsigned int pieces) {
	if (pieces < 2) {
		pieces = 2;
	}
	GridState g;
	unsigned int placed = 0;
	for (unsigned int y = 0; y < gridSize; y++) {
		for (unsigned int x = 0; x < gridSize; x++) {
			if (placed >= pieces) {
				break;
			}
			if (random() < conduitChance) {
				g.addConduit(y, x);
				placed += 1;
			}
		}
		if (placed >= pieces) {
			break;
		}
	}
	CoordinateVector c = g.conduits();
	// transform at least 1 conduit to an electron head
	for (unsigned int i = 0; i < c.size(); i++) {
		if (random() < headChance) {
			g.addHead(c[i]);
		}
		if (i == c.size() - 1 && g.heads().size() == 0) {
			// at least 1 head
			i = 0;
		}
	}
	if (g.conduits().size() < 2) {
		return pickRandom(pieces + 1);
	}
	return g;
}

GridState WebWorker::pickUnion() {
	// FIXME
	return pickRandom(rand() % gridSize * gridSize / 2);
}

WorkCycle WebWorker::updateGrid() {
	size_t prev = this->grid->me();
	size_t next = 0;
	if (this->globalMap->count(prev) > 0) {
		return WorkCycle::GLOBALREPEATED;
	}
	else if ((*this->personalMap)[prev].child != 0) {
		return WorkCycle::REPEATED;
	}
	else if (grid->count() < 2) {
		return WorkCycle::DONE; // give up on this grid.
	}
	else {
		(*this->personalMap)[prev].fitness = this->grid->update();
		next = this->grid->me();
		(*this->personalMap)[prev].child = next;
		(*this->personalMap)[next].parents.push_back(prev);
		(*this->personalMap)[next].state = this->grid->getState();
		if (prev == next) {
			return WorkCycle::REPEATED;
		}
	}
	return WorkCycle::WORKLIVING;
}

WorkCycle WebWorker::dumpData() {
	if (globalMap == nullptr || workMutex == nullptr) {
		return WorkCycle::WORKFAILED;
	}
	if (SDL_LockMutex(workMutex) == 0) {
		std::map<size_t, LifeRecord>::iterator it;
		for (it = personalMap->begin(); it != personalMap->end(); it++) {
			if (it->second.state.conduits().size() < 2) {
				continue;
			}
			if (globalMap->count(it->first) <= 0) {
				(*globalMap)[it->first] = it->second;
			}
			else {
				for (unsigned int i = 0; i < it->second.parents.size(); i++) {
					if (!(*globalMap)[it->first].parentsInclude(it->second.parents[i])) {
						(*globalMap)[it->first].parents.push_back(it->second.parents[i]);
					}
				}
			}
		}

		SDL_UnlockMutex(workMutex);
	}
	else {
		// could not lock mutex
		return WorkCycle::WORKFAILED;
	}
	return WorkCycle::DONE;
}

void WebWorker::clear() {
	grid->clear();
	personalMap->clear();
}

int WebWorker::stop() {
	this->searching = false;
	int result = 0;
	SDL_WaitThread(workerThread, &result);
	return result;
}