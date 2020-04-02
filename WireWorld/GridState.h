#pragma once

#include "CoordinateVector.h"
#include <string>
#include <sstream>
#include <math.h>

class GridState {
	CoordinateVector conduitCoords;
	CoordinateVector headCoords;
	CoordinateVector tailCoords;
public:
	GridState() {
		conduitCoords.clear();
		headCoords.clear();
		tailCoords.clear();
	}
	GridState(CoordinateVector conduits, CoordinateVector heads, CoordinateVector tails) {
		conduitCoords = conduits;
		headCoords = heads;
		tailCoords = tails;
	}
	GridState(const GridState& g) {
		this->conduitCoords = g.conduitCoords;
		this->headCoords = g.headCoords;
		this->tailCoords = g.tailCoords;
	}

	CoordinateVector conduits() {
		return conduitCoords;
	}
	CoordinateVector heads() {
		return headCoords;
	}
	CoordinateVector tails() {
		return tailCoords;
	}

	void setConduits(CoordinateVector conduits) {
		conduitCoords = conduits;
	}
	void addConduit(std::pair<unsigned int, unsigned int> p) {
		conduitCoords.push(p);
	}
	void addConduit(unsigned int y, unsigned int x) {
		addConduit(std::make_pair(y, x));
	}
	void setHeads(CoordinateVector heads) {
		headCoords = heads;
	}
	void addHead(std::pair<unsigned int, unsigned int> p) {
		headCoords.push(p);
	}
	void addHead(unsigned int y, unsigned int x) {
		addHead(std::make_pair(y, x));
	}
	void setTails(CoordinateVector tails) {
		tailCoords = tails;
	}
	void addTail(std::pair<unsigned int, unsigned int> p) {
		tailCoords.push(p);
	}
	void addTail(unsigned int y, unsigned int x) {
		addTail(std::make_pair(y, x));
	}
	void set(CoordinateVector conduits, CoordinateVector heads, CoordinateVector tails) {
		setConduits(conduits);
		setHeads(heads);
		setTails(tails);
	}

	void clearConduits() {
		conduitCoords.clear();
	}
	void clearHeads() {
		headCoords.clear();
	}
	void clearTails() {
		tailCoords.clear();
	}
	void clear() {
		clearConduits();
		clearHeads();
		clearTails();
	}

	bool conduitEquals(const CoordinateVector& c) {
		return conduitCoords == c;
	}
	bool headEquals(const CoordinateVector& c) {
		return headCoords == c;
	}
	bool tailEquals(const CoordinateVector& c) {
		return tailCoords == c;
	}
	bool operator==(const GridState& g) {
		return conduitEquals(g.conduitCoords) && headEquals(g.headCoords) && tailEquals(g.tailCoords);
	}
	bool operator!=(const GridState& g) {
		return !conduitEquals(g.conduitCoords) || !headEquals(g.headCoords) || !tailEquals(g.tailCoords);
	}

	size_t hash() {
		// FIXME this does rely that all coordinates are well ordered for hashes to match...
		std::stringstream stream;
		std::hash<std::string> hashThis;
		stream << "c";
		for (unsigned int i = 0; i < conduitCoords.size(); i++) {
			stream << conduitCoords[i].first << "," << conduitCoords[i].second << "|";
		}
		stream << "h";
		for (unsigned int i = 0; i < headCoords.size(); i++) {
			stream << headCoords[i].first << "," << headCoords[i].second << "|";
		}
		stream << "t";
		for (unsigned int i = 0; i < tailCoords.size(); i++) {
			stream << tailCoords[i].first << "," << tailCoords[i].second << "|";
		}
		return hashThis(stream.str());
	}
};