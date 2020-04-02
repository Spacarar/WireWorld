#pragma once
#include "GridState.h"

class LifeRecord {
public:
	size_t child;
	GridState state;
	std::vector<size_t> parents;
	long fitness;

	bool parentsInclude(size_t identity) {
		for (unsigned int i = 0; i < parents.size(); i++) {
			if (parents[i] == identity) {
				return true;
			}
		}
		return false;
	}

	LifeRecord() {
		child = 0;
		state.clear();
		parents.clear();
		fitness = 0;
	}
	LifeRecord(const LifeRecord& r) {
		fitness = r.fitness;
		child = r.child;
		state = r.state;
		parents.clear();
		for (unsigned int i = 0; i < r.parents.size(); i++) {
			parents.push_back(r.parents[i]);
		}
	}
};