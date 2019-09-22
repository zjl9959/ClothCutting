#pragma once
#ifndef CLOTH_CUTTING_RESULT_H
#define CLOTH_CUTTING_RESULT_H

#include "piece.h"
#include "../utils/myUtils.hpp"

namespace cloth_cutting {

class Vector {
public:
	Vector() {}
	Vector(Coord _x, Coord _y, ID _id, Angle _rotation) : x(_x), y(_y), id(_id), rotation(_rotation) {}
public:
	Coord x;
	Coord y;
	ID id;
	Angle rotation;
	Paths nfp;
};

class Result {
public:
	Result() {}
	Result(List<List<Vector>>& _placements, double &_fitness, List<Piece>& _pieces, double &_area)
		: placements(_placements), fitness(_fitness), pieces(_pieces), area(_area) {}

public:
	List<List<Vector>> placements;
	double fitness;
	List<Piece> pieces;
	double area;
};

}

#endif // !CLOTH_CUTTING_RESULT_H