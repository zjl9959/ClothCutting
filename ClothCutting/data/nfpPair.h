#pragma once
#ifndef CLOTH_CUTTING_NFP_PAIR_H
#define CLOTH_CUTTING_NFP_PAIR_H

#include "piece.h"
#include <libnfporb.hpp>

namespace cloth_cutting {

namespace nfp = libnfporb;

enum Relation { Equals, BOnSegmentA, AOnSegmentB };
enum Type { Inside, Adjacent };

struct polygonRelation {
	ID iA;
	ID iB;
	Relation relation;
};

class NfpPair {
public:
	NfpPair(Piece &a, Piece &b, Type t) : A(a), B(b), type(t) { }
	void nfpPairGenerator();

private:
	void noFitPolygon(polygon_t &res_poly);
	
public:
	Piece& A;
	Piece& B;
	
	Type type;
};


}

#endif // !CLOTH_CUTTING_NFP_PAIR_H