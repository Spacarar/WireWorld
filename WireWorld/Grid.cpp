#include "Grid.h"

Grid::Grid(unsigned int gridSize, unsigned int displaySize) {
	this->gridSize = gridSize;
	this->displaySize = displaySize;
	this->current = new Cell * *[gridSize];
	this->previous = new Cell * *[gridSize];
	for (unsigned int y = 0; y < gridSize; y++) {
		this->current[y] = new Cell * [gridSize];
		this->previous[y] = new Cell * [gridSize];
		for (unsigned int x = 0; x < gridSize; x++) {
			this->current[y][x] = new Cell();
			this->previous[y][x] = new Cell();
		}
	}
}

Grid::Grid(const Grid& g) {
	// remove all elements of existing grid
	for (int y = 0; y < this->gridSize; y++) {
		for (int x = 0; x < this->gridSize; x++) {
			delete this->current[y][x];
			delete this->previous[y][x];
		}
		delete[] this->current[y];
		delete[] this->previous[y];
	}
	delete[] this->current;
	delete[] this->previous;

	// then set all the elements equal to g's elements
	this->gridSize = g.gridSize;
	this->displaySize = g.displaySize;
	this->current = new Cell * *[gridSize];
	this->previous = new Cell * *[gridSize];
	for (int y = 0; y < this->gridSize; y++) {
		this->current[y] = new Cell * [gridSize];
		this->previous[y] = new Cell * [gridSize];
		for (int x = 0; x < gridSize; x++) {
			this->current[y][x] = new Cell(g.current[y][x]->type());
			this->previous[y][x] = new Cell(g.previous[y][x]->type());
		}
	}
}
Grid::~Grid() {
	for (int y = 0; y < this->gridSize; y++) {
		for (int x = 0; x < this->gridSize; x++) {
			delete this->current[y][x];
			delete this->previous[y][x];
		}
		delete[] this->current[y];
		delete[] this->previous[y];
	}
	delete[] this->current;
	delete[] this->previous;
}

unsigned int Grid::safeN(int n) {
	if (n < 0) return this->gridSize - (abs(n) % this->gridSize);
	else if (n < this->gridSize) return n;
	else return n % this->gridSize;
}

Cell** Grid::neighbors(int y, int x) {
	Cell** ee = new Cell*[8];
	ee[0] = current[safeN(y - 1)][safeN(x)];
	ee[1] = current[safeN(y - 1)][safeN(x - 1)];
	ee[2] = current[safeN(y - 1)][safeN(x + 1)];
	ee[3] = current[safeN(y + 1)][safeN(x)];
	ee[4] = current[safeN(y + 1)][safeN(x - 1)];
	ee[5] = current[safeN(y + 1)][safeN(x + 1)];
	ee[6] = current[safeN(y)][safeN(x - 1)];
	ee[7] = current[safeN(y)][safeN(x + 1)];
	return ee;
}

void Grid::draw(SDL_Renderer* ren) {
	static SDL_Rect drawRect;
	drawRect.w = drawRect.h = displaySize;
	for (unsigned int y = 0; y < gridSize; y++) {
		for (unsigned int x = 0; x < gridSize; x++) {
			if (current[y][x]->isActive()) {
				if (current[y][x]->isType(CellType::c_conduit)) {
					SDL_SetRenderDrawColor(ren, 230, 100, 60, 255);
				}
				else if (current[y][x]->isType(CellType::c_head)) {
					SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
				}
				else if (current[y][x]->isType(CellType::c_tail)) {
					SDL_SetRenderDrawColor(ren, 120, 160, 255, 255);
				}
				drawRect.x = x * displaySize;
				drawRect.y = y * displaySize;
				SDL_RenderFillRect(ren, &drawRect);
			}
		}
	}
	SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
}

long Grid::update() {
	long fitness = 0;
	CellType nextType = CellType::c_empty;
	for (unsigned int y = 0; y < gridSize; y++) {
		for (unsigned int x = 0; x < gridSize; x++) {
			Cell** ee = neighbors(y, x);
			fitness += current[y][x]->update(ee, nextType);
			delete[] ee; // do not delete inner members, they are part of the grid
			previous[y][x]->setType(nextType);
		}
	}
	std::swap(current, previous);
	return fitness;
}

size_t Grid::me() {
	return getState().hash();
}

bool Grid::isEmpty() {
	for (unsigned int y = 0; y < gridSize; y++) {
		for (unsigned int x = 0; x < gridSize; x++) {
			if (current[y][x]->isActive()) {
				return false;
			}
		}
	}
	return true;
}

bool Grid::isStagnant() {
	for (unsigned int y = 0; y < gridSize; y++) {
		for (unsigned int x = 0; x < gridSize; x++) {
			if (current[y][x]->isType(CellType::c_head)) {
				return false;
			}
		}
	}
	return true;
}

unsigned int Grid::conduitCount() {
	unsigned int count = 0;
	for (unsigned int y = 0; y < gridSize; y++) {
		for (unsigned int x = 0; x < gridSize; x++) {
			if (current[y][x]->isType(CellType::c_conduit)) {
				count++;
			}
		}
	}
	return count;
}
unsigned int Grid::headCount() {
	unsigned int count = 0;
	for (unsigned int y = 0; y < gridSize; y++) {
		for (unsigned int x = 0; x < gridSize; x++) {
			if (current[y][x]->isType(CellType::c_head)) {
				count++;
			}
		}
	}
	return count;
}
unsigned int Grid::tailCount() {
	unsigned int count = 0;
	for (unsigned int y = 0; y < gridSize; y++) {
		for (unsigned int x = 0; x < gridSize; x++) {
			if (current[y][x]->isType(CellType::c_tail)) {
				count++;
			}
		}
	}
	return count;
}
unsigned int Grid::count() {
	unsigned int count = 0;
	for (unsigned int y = 0; y < gridSize; y++) {
		for (unsigned int x = 0; x < gridSize; x++) {
			if (current[y][x]->isActive()) {
				count++;
			}
		}
	}
	return count;
}

void Grid::clear() {
	for (unsigned int y = 0; y < gridSize; y++) {
		for (unsigned int x = 0; x < gridSize; x++) {
			current[y][x]->setType(CellType::c_empty);
		}
	}
}

void Grid::setState(GridState& g) {
	this->clear();
	CoordinateVector conduits = g.conduits();
	CoordinateVector heads = g.heads();
	CoordinateVector tails = g.tails();
	for (unsigned int i = 0; i < conduits.size(); i++) {
		current[safeN(conduits[i].first)][safeN(conduits[i].second)]->setType(CellType::c_conduit);
	}
	for (unsigned int i = 0; i < heads.size(); i++) {
		current[safeN(heads[i].first)][safeN(heads[i].second)]->setType(CellType::c_head);
	}
	for (unsigned int i = 0; i < tails.size(); i++) {
		current[safeN(tails[i].first)][safeN(tails[i].second)]->setType(CellType::c_tail);
	}
}

GridState Grid::getState() {
	CoordinateVector conduits;
	CoordinateVector heads;
	CoordinateVector tails;
	for (unsigned int y = 0; y < gridSize; y++) {
		for (unsigned int x = 0; x < gridSize; x++) {
			
			if (current[y][x]->isType(CellType::c_conduit)) {
				conduits.push(y, x);
			}
			else if (current[y][x]->isType(CellType::c_head)) {
				heads.push(y, x);
			}
			else if (current[y][x]->isType(CellType::c_tail)) {
				tails.push(y, x);
			}
		}
	}
	GridState state(conduits, heads, tails);
	return state;
}

bool Grid::isType(unsigned int y, unsigned int x, CellType type) {
	return current[safeN(y)][safeN(x)]->isType(type);
}
bool Grid::isConduit(unsigned int y, unsigned int x) {
	return isType(y, x, CellType::c_conduit);
}
bool Grid::isHead(unsigned int y, unsigned int x) {
	return isType(y, x, CellType::c_head);
}
bool Grid::isTail(unsigned int y, unsigned int x) {
	return isType(y, x, CellType::c_tail);
}
bool Grid::isEmpty(unsigned int y, unsigned int x) {
	return isType(y, x, CellType::c_empty);
}

bool Grid::operator==(const Grid& g) {
	if (this->gridSize != g.gridSize) {
		return false;
	}
	for (unsigned int y = 0; y < gridSize; y++) {
		for (unsigned int x = 0; x < gridSize; x++) {
			if (*current[y][x] != *g.current[y][x]) {
				return false;
			}
		}
	}
	return true;
}

bool Grid::operator!=(const Grid& g) {
	if (this->gridSize != g.gridSize) {
		return true;
	}
	for (unsigned int y = 0; y < gridSize; y++) {
		for (unsigned int x = 0; x < gridSize; x++) {
			if (*current[y][x] != *g.current[y][x]) {
				return true;
			}
		}
	}
	return false;
}

