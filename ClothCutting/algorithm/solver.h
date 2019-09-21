#pragma once
#ifndef CLOTH_CUTTING_SOLVER_H
#define CLOTH_CUTTING_SOLVER_H

#include "problem.h"
#include "../Config.h"
#include "../data/piece.h"
#include "../data/nfpPair.h"
#include "../data/result.h"
#include "../utils/myUtils.hpp"

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
	bool rotateCheck(const box_t &bin, Piece &piece);
	void greedyWorker(const box_t &bin, const List<ID>& candidate_index);
	Result placeWorker(const box_t &bin, const List<Piece>& pieces, const HashMap<String, polygon_t>& nfp_cache);

protected:
    Input &input;
	List<box_t> bins; // √Ê¡œ
	List<Piece> pieces; // ¡„º˛
	HashMap<ID, ID> pieceIdMap; // id”≥…‰£¨idMap[piece_id]  = item_id
	Config config;
};

}

#endif // !CLOTH_CUTTING_SOLVER_H
