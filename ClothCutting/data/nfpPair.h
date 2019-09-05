#pragma once
#ifndef CLOTH_CUTTING_NFP_PAIR_H
#define CLOTH_CUTTING_NFP_PAIR_H

#include "piece.h"

namespace cloth_cutting {
class NfpPair {
public:
	NfpPair(Piece &a, Piece &b) : A(a), B(b), type(Type::Inside) { }

public:
	Piece& A;
	Piece& B;
	enum Type { Inside, Adjacent } type;
};


}

#endif // !CLOTH_CUTTING_NFP_PAIR_H