#pragma once

#ifndef CLOTH_CUTTING_BASE_PATH_HPP
#define CLOTH_CUTTING_BASE_PATH_HPP

#include "../Common.h"
#include "../utils/boostUtils.hpp"

namespace cloth_cutting {

class BasePath {
public:
	BasePath() : 
		offsetX(0), offsetY(0), area(0) {}

private:
	List<segment_t> segments;
	ID id;
	int source; // ? 
	int rotation;

public:
	double offsetX;
	double offsetY;
	double area;
};

}


#endif // !CLOTH_CUTTING_BASEPATH_HPP
