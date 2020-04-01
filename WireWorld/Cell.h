#pragma once
#include "Global.h"

class Cell {
	CellType _type;

public:
	Cell(CellType type = CellType::c_empty);
	Cell(const Cell& c);

	// let the cell update itself based on neighboring cells
	// return fitness of cell
	long update(Cell* neighbors[8], CellType& nextType);
	
	CellType type();
	void setType(CellType t);
	bool isType(CellType t);
	bool isActive();

	bool operator==(const Cell& c);
	bool operator!=(const Cell& c);
};