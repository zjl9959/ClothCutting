#pragma once
#ifndef CLOTH_CUTTING_PIECE_H
#define CLOTH_CUTTING_PIECE_H

#include "../Common.h"
#include "../Config.h"
#include "../utils/boostUtils.hpp"
#include "../utils/clipperUtils.hpp"

namespace cloth_cutting {

class Piece {
public:
	Piece() : offsetX(0), offsetY(0) {}
	void cleanPiece(Piece &res_piece) const;
	void offsetPiece(List<Piece>& res_pieces, double offset) const;
	
public:
	ID id;
	polygon_t poly;
	Angle rotation; // 0, 90, 180, 270, 360
	double offsetX;
	double offsetY;
};

}


#endif // !CLOTH_CUTTING_PIECE_H
