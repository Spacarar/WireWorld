#pragma once

#include <SDL.h>
#include "Cell.h"
#include "GridState.h"
#include "Global.h"

class Grid {
	Cell*** current;
	Cell*** previous;

	unsigned int gridSize;
	unsigned int displaySize;

	unsigned int safeN(int n);

	Cell** neighbors(int y, int x); //[8]

public:
	Grid(unsigned int gridSize = 4, unsigned int displaySize = 1);
	Grid(const Grid& g);
	~Grid();

	void draw(SDL_Renderer* ren);
	long update();
	
	size_t me();
	
	bool isEmpty(); // no cells
	bool isStagnant(); // no heads
	
	unsigned int conduitCount();
	unsigned int headCount();
	unsigned int tailCount();
	unsigned int count(); // any living cell type

	void clear();
	
	void setState(GridState& g);
	GridState getState();

	bool isType(unsigned int y, unsigned int x, CellType type);
	bool isConduit(unsigned int y, unsigned int x);
	bool isHead(unsigned int y, unsigned int x);
	bool isTail(unsigned int y, unsigned int x);
	bool isEmpty(unsigned int y, unsigned int x);

	bool operator==(const Grid& g);
	bool operator!=(const Grid& g);
};