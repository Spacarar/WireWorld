#pragma once

#include <vector>

class CoordinateVector {
	std::vector<std::pair<unsigned int, unsigned int> > v;

public:
	CoordinateVector() {
		v.clear();
	}
	CoordinateVector(std::vector<std::pair<unsigned int, unsigned int> > vec) {
		v.clear();
		for (unsigned int i = 0; i < vec.size(); i++) {
			v.push_back(vec[i]);
		}
	}
	CoordinateVector(const CoordinateVector& c) {
		v.clear();
		for (unsigned int i = 0; i < c.v.size(); i++) {
			v.push_back(c.v[i]);
		}
	}

	void set(std::vector<std::pair<unsigned int, unsigned int> > vec) {
		v.clear();
		for (unsigned int i = 0; i < vec.size(); i++) {
			v.push_back(vec[i]);
		}
	}

	void push(unsigned int y, unsigned int x) {
		push(std::make_pair(y, x));
	}
	void push(std::pair<unsigned int, unsigned int> p) {
		v.push_back(p);
	}

	void clear() {
		v.clear();
	}
	size_t size() {
		return v.size();
	}

	bool includes(unsigned int y, unsigned int x) const {
		includes(std::make_pair(y, x));
	}
	bool includes(std::pair<unsigned int, unsigned int> p) const {
		for (unsigned int i = 0; i < v.size(); i++) {
			if (v[i].first == p.first && v[i].second == p.second) {
				return true;
			}
		}
		return false;
	}

	bool operator==(const CoordinateVector& c) {
		if (v.size() != c.v.size()) {
			return false;
		}
		for (unsigned int i = 0; i < v.size(); i++) {
			if (!c.includes(v[i])) {
				return false;
			}
		}
		return true;
	}

	bool operator!=(const CoordinateVector& c) {
		if (v.size() != c.v.size()) {
			return true;
		}
		for (unsigned int i = 0; i < v.size(); i++) {
			if (!c.includes(v[i])) {
				return true;
			}
		}
		return false;
	}

	std::pair<unsigned int, unsigned int>& operator[](size_t ind) {
		return v[ind];
	}
	std::pair<unsigned int, unsigned int> operator[](size_t ind) const {
		return v[ind];
	}
};