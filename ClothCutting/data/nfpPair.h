#pragma once
#ifndef CLOTH_CUTTING_NFP_PAIR_H
#define CLOTH_CUTTING_NFP_PAIR_H

#include "piece.h"

namespace cloth_cutting {

static String getNfpKey(const Piece &A, const Piece &B) {
	return std::to_string(A.id) 
		+ '_' + std::to_string(B.id) 
		+ '_' + std::to_string(A.rotation) 
		+ '_' + std::to_string(B.rotation);
}

static String getNfpKey(const Piece &A) {
	return std::to_string(A.id) + '_' + std::to_string(A.rotation);
}

class NfpPair {
public:
	NfpPair(const Piece &a, const Piece &b) : A(a), B(b) { nfp_key = getNfpKey(A, B); }
	NfpPair(const Piece &a, const box_t &b) : A(a), bin(b) { nfp_key = getNfpKey(A); }
	void nfpPairGenerator();
	
public:
	box_t bin;
	Piece A;
	Piece B;
	polygon_t nfp;
	String nfp_key;
};

}

#endif // !CLOTH_CUTTING_NFP_PAIR_H