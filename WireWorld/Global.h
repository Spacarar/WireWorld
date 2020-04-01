#pragma once
#include <stdlib.h>
#include <time.h>

enum class CellType {c_empty, c_conduit, c_head, c_tail};

// on random initialization
// no need to initialize tails.
// remaining percent or missed chance populate empty
const float conduitChance = 0.60;
const float headChance = 0.20;

// cell fitness constants
const long F_HEAD_TAIL = 10;
const long F_TAIL_CONDUIT = 5;
const long F_CONDUIT_HEAD = 100;
const long F_CONDUIT_CONDUIT = 1;

// unnamed namespace for global functions
namespace {
	void initRand() { // use once.
		srand(time(NULL));
	}

	float random() { // 0-1 float inclusive
		return static_cast<float> (rand() / static_cast<float> (RAND_MAX));
	}
}