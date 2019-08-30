#pragma once
#ifndef CLOTH_CUTTING_SOLVER_H
#define CLOTH_CUTTING_SOLVER_H

#include "problem.h"
#include "../Config.h"
#include "../utils/boostUtils.hpp"

namespace cloth_cutting {

class Solver {
public:
    Solver(Input &_input) : input(_input) {};
    ~Solver() {};
	void run();

private:
	void preprocess();

protected:
    Input &input;
	List<box_t> bins; // √Ê¡œ
	List<polygon_t> pieces; // ¡„º˛
	HashMap<ID, ID> pieceIdMap; // id”≥…‰£¨idMap[piece_id]  = item_id
	Config config;
};

}

#endif // !CLOTH_CUTTING_SOLVER_H
