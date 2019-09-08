#include "Solver.h"
#include <random>
#include <chrono>

namespace cloth_cutting {
	
void Solver::run(){
	List<Piece> origin_pieces;
	preprocess(origin_pieces);
	List<Piece> clean_pieces;
	cleanAll(origin_pieces, clean_pieces);
	offsetAll(clean_pieces, this->pieces);
	for (auto iter = bins.begin(); iter != bins.end(); ++iter) {
		List<ID> candidate_index = placeCheck(*iter);
		greedyWorker(*iter, candidate_index);
	}
}

void Solver::preprocess(List<Piece>& out_pieces) {
	// plate -> bin
	bins.reserve(input.plates.size());
	for (auto &each_plate : input.plates) {
		auto ox = static_cast<Coord>(each_plate.minPadding);
		auto oy = static_cast<Coord>(each_plate.minPadding);
		auto x = static_cast<Coord>(each_plate.length - each_plate.minPadding);
		auto y = static_cast<Coord>(each_plate.width - each_plate.minPadding);
		bins.emplace_back(point_t(ox, oy), point_t(x, y));
	}

	// item -> piece
	ID piece_index = 0;
	out_pieces.reserve(input.items.size() * 2);
	for (int i = 0; i < input.items.size(); ++i) {
		int amount = input.items[i].amount;
		while (amount) {
			Piece piece;
			for (auto &each_xy : input.items[i].raw_coords) {
				bg::append(piece.poly.outer(), point_t(each_xy.x, each_xy.y)); // 暂时只考虑外环
			}
			// 平移多边形包络左下角到坐标原点处，减少坐标数值
			box_t envelope = bg::return_envelope<box_t>(piece.poly);
			auto offsetX = bg::get<bg::min_corner, 0>(envelope); // auto offsetX2 = envelope.min_corner().x();
			auto offsetY = bg::get<bg::min_corner, 1>(envelope); // auto offsetY2 = envelope.min_corner().y();
			piece.poly.clear();
			for (auto &each_xy : input.items[i].raw_coords) {
				bg::append(piece.poly.outer(), point_t(each_xy.x - offsetX, each_xy.y - offsetY));
			}
			piece.id = piece_index;
			pieceIdMap[piece_index++] = i;
			out_pieces.push_back(piece);
			--amount;
		}
	}
}

/*
* 删除共线、半邻接的坐标点
*/
void Solver::cleanAll(const List<Piece>& in_pieces, List<Piece>& out_pieces) {
	out_pieces.reserve(in_pieces.size());
	for (int i = 0; i < in_pieces.size(); ++i) {
		Piece clean_piece;
		in_pieces[i].cleanPiece(clean_piece);
		if (clean_piece.poly.outer().size() > 3 && bg::area(clean_piece.poly)) {
			out_pieces.push_back(clean_piece);
		}
		else {
			std::cout << "origin piece " << i << " was deleted." << std::endl;
		}
	}
}

/*
* 缩放（向外膨胀）全部多边形
*/
void Solver::offsetAll(const List<Piece>& in_pieces, List<Piece>& out_pieces) {
	out_pieces.reserve(in_pieces.size());
	for (int i = 0; i < in_pieces.size(); ++i) {
		List<Piece> offset_pieces;
		in_pieces[i].offsetPiece(offset_pieces, 0.5 * config.minGap);
		out_pieces.push_back(offset_pieces[0]);
	}
}

/*
* 检索可以放置在 bin 上的 piece
*/
List<ID> Solver::placeCheck(const box_t &bin) {
	auto bin_width = bg::get<bg::max_corner, 0>(bin) - bg::get<bg::min_corner, 0>(bin);
	auto bin_height = bg::get<bg::max_corner, 1>(bin) - bg::get<bg::min_corner, 1>(bin);
	List<ID> candidate_index; candidate_index.reserve(pieces.size());
	for (auto iter = pieces.begin(); iter != pieces.end(); ++iter) {
		for (Angle each_angle : input.items[pieceIdMap[(*iter).id]].rotateAngles) {
			bg::strategy::transform::rotate_transformer<bg::degree, int, 2, 2> rotate(each_angle);
			polygon_t rotated_poly;
			bg::transform((*iter).poly, rotated_poly, rotate);
			box_t envelope = bg::return_envelope<box_t>(rotated_poly);
			auto width = bg::get<bg::max_corner, 0>(envelope) - bg::get<bg::min_corner, 0>(envelope);
			auto height = bg::get<bg::max_corner, 1>(envelope) - bg::get<bg::min_corner, 1>(envelope);
			if (width < bin_width && height < bin_height) {
				candidate_index.push_back(iter - pieces.begin());
				break;
			}
		}
	}
	return candidate_index;
}

/*
* 随机返回一个合法的旋转角度
*/
Angle Solver::rotateCheck(const box_t & bin, const Piece & piece) {
	auto bin_width = bg::get<bg::max_corner, 0>(bin) - bg::get<bg::min_corner, 0>(bin);
	auto bin_height = bg::get<bg::max_corner, 1>(bin) - bg::get<bg::min_corner, 1>(bin);
	List<Angle> candidate_angles = input.items[pieceIdMap[piece.id]].rotateAngles;
	auto seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::shuffle(candidate_angles.begin(), candidate_angles.end(), std::default_random_engine(seed));
	for (Angle each_angle : candidate_angles) {
		polygon_t rotate_poly;
		rotatePolygon(piece.poly, rotate_poly, each_angle);
		box_t envelope = bg::return_envelope<box_t>(rotate_poly);
		auto width = bg::get<bg::max_corner, 0>(envelope) - bg::get<bg::min_corner, 0>(envelope);
		auto height = bg::get<bg::max_corner, 1>(envelope) - bg::get<bg::min_corner, 1>(envelope);
		if (width < bin_width && height < bin_height) {
			return each_angle;
		}
	}
}

/*
* 将候选物品 candidate_index 放到 bin 上
*/
void Solver::greedyWorker(const box_t &bin, const List<ID>& candidate_index) {
	List<Piece> candidate_pieces;
	for (ID index : candidate_index) {
		candidate_pieces.push_back(pieces[index]);
	}
	sort(candidate_pieces.begin(), candidate_pieces.end(),
		[](Piece &lhs, Piece &rhs) { return bg::area(lhs.poly) < bg::area(rhs.poly); });
	for (auto iter = candidate_pieces.begin(); iter != candidate_pieces.end(); ++iter) {
		(*iter).rotation = rotateCheck(bin, *iter);
	}
	
	List<NfpPair> nfp_pairs;
	for (int i = 0; i < candidate_pieces.size(); ++i) {
		nfp_pairs.emplace_back(bin, candidate_pieces[i]);
		for (int j = 0; j < i; ++j) {
			nfp_pairs.emplace_back(candidate_pieces[j], candidate_pieces[i]);
		}
	}
	// 生成 nfp
	for (NfpPair &nfpPair : nfp_pairs) {
		ParallelData data = nfpGenerator(nfpPair, config);
	}
	for (int i = 0; i < generatedNfp.size(); i++) {
		ParallelData Nfp = generatedNfp.get(i);
		//TODO remove gson & generate a new key algorithm
		String tkey = gson.toJson(Nfp.getKey());
		nfpCache.put(tkey, Nfp.value);
	}


	Placementworker worker = new Placementworker(binPolygon, config, nfpCache);
	List<NestPath> placeListSlice = new ArrayList<NestPath>();



	for (int i = 0; i < placelist.size(); i++) {
		placeListSlice.add(new NestPath(placelist.get(i)));
	}
	List<List<NestPath>> data = new ArrayList<List<NestPath>>();
	data.add(placeListSlice);
	List<Result> placements = new ArrayList<Result>();
	for (int i = 0; i < data.size(); i++) {
		Result result = worker.placePaths(data.get(i));
		placements.add(result);
	}
	if (placements.size() == 0) {
		return null;
	}
	individual.fitness = placements.get(0).fitness;
	Result bestResult = placements.get(0);
	for (int i = 1; i < placements.size(); i++) {
		if (placements.get(i).fitness < bestResult.fitness) {
			bestResult = placements.get(i);
		}
	}
	return bestResult;

}

}
