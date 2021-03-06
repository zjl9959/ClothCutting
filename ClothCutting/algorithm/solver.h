#pragma once
#ifndef CLOTH_CUTTING_SOLVER_H
#define CLOTH_CUTTING_SOLVER_H

#include "problem.h"
#include "../Config.h"
#include "../data/piece.h"
#include "../data/nfpPair.h"
#include "../data/vector.hpp"
#include "../utils/myUtils.hpp"

namespace cloth_cutting {

class Solver {
public:
    Solver(Input &_input) : input(_input) {};
    ~Solver() {};
	void run();
	void saveOutput();

private:
	void preprocess(List<Piece>& origin_pieces);
	void zeroAll(const List<Piece>& in_pieces, List<Piece>& out_pieces);
	void cleanAll(const List<Piece>& in_pieces, List<Piece>& out_pieces);
	void offsetAll(const List<Piece>& in_pieces, List<Piece>& out_pieces);
	List<ID> placeCheck(const box_t &bin);
	bool rotateCheck(const box_t &bin, Piece &piece);
	void greedyWorker(const box_t &bin, const List<ID>& candidate_index);
	double placeWorker(const box_t &bin, const HashMap<String, polygon_t>& nfp_cache, 
		List<Piece>& candidate_pieces, List<Piece>& placed_pieces, List<Vector>& placed_vectors);

protected:
    Input &input;
	List<box_t> bins; // ����
	List<Piece> pieces; // ���
	HashMap<ID, ID> pieceIdMap; // idӳ�䣬idMap[piece_id]  = item_id
	Config config;
};

}

#endif // !CLOTH_CUTTING_SOLVER_H
