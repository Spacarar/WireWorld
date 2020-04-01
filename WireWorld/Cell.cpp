#include "Cell.h"

Cell::Cell(CellType type) {
	this->_type = type;
}

Cell::Cell(const Cell& c) {
	this->_type = c._type;
}

CellType Cell::type() {
	return this->_type;
}
void Cell::setType(CellType t) {
	this->_type = t;
}
bool Cell::isType(CellType t) {
	return this->_type == t;
}
bool Cell::isActive() {
	return this->_type != CellType::c_empty;
}

bool Cell::operator==(const Cell& c) {
	return this->_type == c._type;
}
bool Cell::operator!=(const Cell& c) {
	return this->type != c._type;
}

long Cell::update(Cell* neighbors[8], CellType& nextType) {
	long fitness = 0;
	if (this->_type == CellType::c_empty) {
		nextType = CellType::c_empty;
	}
	else if (this->_type == CellType::c_tail) {
		// a tail will ALWAYS become a conduit
		nextType = CellType::c_conduit;
		fitness = F_TAIL_CONDUIT;
	}
	else if (this->_type == CellType::c_head) {
		// a head will ALWAYS become a tail
		nextType = CellType::c_tail;
		fitness = F_HEAD_TAIL;
	}
	else if (this->_type == CellType::c_conduit) {
		// a conduit will become a head if 1 or 2 heads are near it
		int headCount = 0;
		for (int i = 0; i < 8; i++) {
			if (neighbors[i]->isType(CellType::c_head)) {
				headCount += 1;
			}
		}
		if (headCount == 1 || headCount == 2) {
			nextType = CellType::c_head;
			fitness = F_CONDUIT_HEAD;
		}
		else {
			nextType = CellType::c_conduit;
			fitness = F_CONDUIT_CONDUIT;
		}
	}
	return fitness;
}