#pragma once
#ifndef CLOTH_CUTTING_MY_UTILS_HPP
#define CLOTH_CUTTING_MY_UTILS_HPP

#include <cmath>

namespace cloth_cutting {

const double TOL = pow(10, -2);

static bool almostEqual(double a, double b) {
	return abs(a - b) < TOL;
}

}


#endif // !CLOTH_CUTTING_MY_UTILS_HPP