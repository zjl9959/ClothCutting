#pragma once
#ifndef CLOTH_CUTTING_PIECE_H
#define CLOTH_CUTTING_PIECE_H

#include "../utils/clipperUtils.hpp"

namespace cloth_cutting {

class Piece {
public:
	Piece() : rotation(0), offsetX(0), offsetY(0) {}
	void zeroPiece(Piece &res_piece) const;
	void cleanPiece(Piece &res_piece) const;
	void offsetPiece(Piece &res_piece, double offset) const;
	inline bool operator ==(const Piece &a) { return this->id == a.id; };
	
public:
	ID id;
	polygon_t poly;
	Angle rotation; // { 0, 90, 180, 270, 360 }
	double offsetX;
	double offsetY;
};

}

#endif // !CLOTH_CUTTING_PIECE_H
