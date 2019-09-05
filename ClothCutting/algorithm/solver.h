#pragma once
#ifndef CLOTH_CUTTING_SOLVER_H
#define CLOTH_CUTTING_SOLVER_H

#include "problem.h"
#include "../Config.h"
#include "../data/piece.h"
#include "../data/nfpPair.h"
#include "../utils/boostUtils.hpp"
#include "../utils/clipperUtils.hpp"

namespace cloth_cutting {

class Solver {
public:
    Solver(Input &_input) : input(_input) {};
    ~Solver() {};
	void run();

private:
	void preprocess(List<Piece>& out_pieces);
	void cleanAll(const List<Piece>& in_pieces, List<Piece>& out_pieces);
	void offsetAll(const List<Piece>& in_pieces, List<Piece>& out_pieces);
	List<ID> placeCheck(const box_t &bin);
	Angle rotateCheck(const box_t &bin, const Piece &piece);
	void greedyWorker(const box_t &bin, const List<ID>& candidate_index);

protected:
    Input &input;
	List<box_t> bins; // √Ê¡œ
	List<Piece> pieces; // ¡„º˛
	HashMap<ID, ID> pieceIdMap; // id”≥…‰£¨idMap[piece_id]  = item_id
	Config config;
};

}

#endif // !CLOTH_CUTTING_SOLVER_H
