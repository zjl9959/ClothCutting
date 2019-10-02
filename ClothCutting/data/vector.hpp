#pragma once
#ifndef CLOTH_CUTTING_VECTOR_HPP
#define CLOTH_CUTTING_VECTOR_HPP

#include "../Common.h"

namespace cloth_cutting {

class Vector {
public:
	Vector() {}
	Vector(ID _id, Coord _x, Coord _y, Angle _rotation) : id(_id), x(_x), y(_y), rotation(_rotation) {}
public:
	ID id;
	Coord x;
	Coord y;
	Angle rotation;
};

}

#endif // !CLOTH_CUTTING_VECTOR_HPP