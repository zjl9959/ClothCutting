#pragma once
#ifndef CLOTH_CUTTING_SOLVER_H
#define CLOTH_CUTTING_SOLVER_H

#include "problem.h"
#include "../Common.h"

namespace cloth_cutting {

class Solver {
public:
    Solver(Input &_input) : input(_input) {};
    ~Solver() {};
protected:
    Input &input;

};

}

#endif // !CLOTH_CUTTING_SOLVER_H
