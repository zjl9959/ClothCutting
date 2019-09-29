#include "Solver.h"
#include <random>
#include <chrono>

namespace cloth_cutting {
	
void Solver::run(){
	List<Piece> origin_pieces, zero_pieces, offset_pieces, clean_pieces;
	preprocess(origin_pieces);
	zeroAll(origin_pieces, zero_pieces);
	offsetAll(zero_pieces, offset_pieces); 
	cleanAll(offset_pieces, this->pieces);
	//cleanAll(zero_pieces, clean_pieces); 
	//offsetAll(clean_pieces, this->pieces);
	auto bin = bins[0];
	List<ID> candidate_index = placeCheck(bin);
	greedyWorker(bin, candidate_index);
}

void Solver::preprocess(List<Piece>& origin_pieces) {
	// plate -> bin
	bins.reserve(input.plates.size());
	for (auto &each_plate : input.plates) {
		auto ox = static_cast<Coord>(each_plate.minPadding);
		auto oy = static_cast<Coord>(each_plate.minPadding);
		auto x = static_cast<Coord>(each_plate.width - each_plate.minPadding);
		auto y = static_cast<Coord>(each_plate.height - each_plate.minPadding);
		bins.emplace_back(point_t(ox, oy), point_t(x, y));
	}

	// item -> piece
	ID piece_index = 0;
	origin_pieces.reserve(input.items.size());
	for (int i = 0; i < input.items.size(); ++i) {
		int amount = input.items[i].amount;
		while (amount) {
			Piece piece;
			for (auto &each_xy : input.items[i].raw_coords) {
				bg::append(piece.poly.outer(), point_t(each_xy.x, each_xy.y)); // 算例只有外环
			}
			piece.id = piece_index;
			pieceIdMap[piece_index++] = i;
			origin_pieces.push_back(piece);
			--amount;
		}
	}

	// todo Defect
}

/*
* 将所有多边形平移到坐标原点处
*/
void Solver::zeroAll(const List<Piece>& in_pieces, List<Piece>& out_pieces) {
	out_pieces.reserve(in_pieces.size());
	for (auto &in_piece : in_pieces) {
		Piece zero_piece;
		in_piece.zeroPiece(zero_piece);
		out_pieces.push_back(zero_piece);
	}
}

/*
* 所有多边形简化
*/
void Solver::cleanAll(const List<Piece>& in_pieces, List<Piece>& out_pieces) {
	out_pieces.reserve(in_pieces.size());
	for (auto &in_piece : in_pieces) {
		Piece clean_piece;
		in_piece.cleanPiece(clean_piece);
		out_pieces.push_back(clean_piece);
	}
}

/*
* 所有多边形向外膨胀 1/2 Gap
*/
void Solver::offsetAll(const List<Piece>& in_pieces, List<Piece>& out_pieces) {
	out_pieces.reserve(in_pieces.size());
	for (auto &in_piece : in_pieces) {
		Piece offset_piece;
		in_piece.offsetPiece(offset_piece, 0.5 * config.minGap);
		out_pieces.push_back(offset_piece);
	}
}

/*
* 检索可以放置在 bin 上的 pieces 候选集合
*/
List<ID> Solver::placeCheck(const box_t &bin) {
	auto bin_width = bin.max_corner().x() - bin.min_corner().x();
	auto bin_height = bin.max_corner().y() - bin.min_corner().y();
	List<ID> candidate_index; candidate_index.reserve(pieces.size());
	for (auto & piece : pieces) {
		for (Angle each_angle : input.items[pieceIdMap[piece.id]].rotateAngles) {
			polygon_t rotate_poly;
			rotatePolygon(piece.poly, rotate_poly, each_angle);
			box_t envelope;
			rectangle_t rect = getEnvelope(rotate_poly, envelope);
			auto width = rect.first, height = rect.second;
			if (width < bin_width && height < bin_height) {
				candidate_index.push_back(piece.id);
				break;
			}
		}
	}
	return candidate_index;
}

/*
* 随机一个合法的旋转角度，执行旋转操作
*/
bool Solver::rotateCheck(const box_t & bin, Piece & piece) {
	auto bin_width = bin.max_corner().x() - bin.min_corner().x();
	auto bin_height = bin.max_corner().y() - bin.min_corner().y();
	List<Angle> candidate_angles = input.items[pieceIdMap[piece.id]].rotateAngles;
	// candidate_angles 列表洗牌
	auto seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::shuffle(candidate_angles.begin(), candidate_angles.end(), std::default_random_engine(seed));
	for (Angle each_angle : candidate_angles) {
		polygon_t rotate_poly;
		rotatePolygon(piece.poly, rotate_poly, each_angle);
		box_t envelope;
		rectangle_t rect = getEnvelope(rotate_poly, envelope);
		auto width = rect.first, height = rect.second;
		if (width < bin_width && height < bin_height) {
			piece.rotation = each_angle;
			piece.poly = rotate_poly; // 执行旋转操作
			return true;
		}
	}
	return false;
}

/*
* 一次贪心迭代
*/
void Solver::greedyWorker(const box_t &bin, const List<ID>& candidate_index) {
	List<Piece> candidate_pieces;
	for (ID index : candidate_index) { candidate_pieces.push_back(pieces[index]); }
	sort(candidate_pieces.begin(), candidate_pieces.end(),
		[](Piece &lhs, Piece &rhs) { return bg::area(lhs.poly) > bg::area(rhs.poly); });
	for (auto &piece : candidate_pieces) {
		if (!rotateCheck(bin, piece)) { // 此时多边形已旋转
			std::cout << "rotate piece " << piece.id << " has no legal angle." << std::endl;
		}
	}
	List<NfpPair> nfp_pairs; 
	for (int i = 0; i < candidate_pieces.size(); ++i) {
		// Inner Fit Polygon
		NfpPair infp_pair(candidate_pieces[i], bin);
		infp_pair.nfpPairGenerator();
		nfp_pairs.push_back(infp_pair);
		// No Fit Polygon
		for (int j = 0; j < i; ++j) {
			NfpPair nfp_pair(candidate_pieces[j], candidate_pieces[i]);
			nfp_pair.nfpPairGenerator();
			nfp_pairs.push_back(nfp_pair);
		}
	}
	HashMap<String, polygon_t> nfp_cache;
	for (auto &each_pair : nfp_pairs) { nfp_cache[each_pair.nfp_key] = each_pair.nfp; }

	List<Piece> placed_pieces;   // 已放置的零件
	List<Vector> placed_vectors; // 对应矢量
	placeWorker(bin, nfp_cache, candidate_pieces, placed_pieces, placed_vectors);

#pragma region ApplyPlacement
	for (auto & each_vector : placed_vectors) {
		this->pieces[each_vector.id].offsetX = each_vector.x;
		this->pieces[each_vector.id].offsetY = each_vector.y;
		this->pieces[each_vector.id].rotation = each_vector.rotation;
	}
#pragma endregion ApplyPlacement
}

/*
* 将候选物品 candidate_pieces 放到 bin 上
* List<Piece> placed_pieces;   // 已放置的零件
* List<Vector> placed_vectors; // 对应矢量
*/
double Solver::placeWorker(const box_t &bin, const HashMap<String, polygon_t>& nfp_cache, 
						   List<Piece>& candidate_pieces, List<Piece>& placed_pieces, List<Vector>& placed_vectors) {
	double fitness = 0;  // 种群算法中种群的适应度
	
	double min_obj_width = DistanceMax;  // 目标：最小化当前已使用的面料宽度，即最右侧坐标
	
	// 开始一块新零件的放置
	for (auto &curr_piece : candidate_pieces) {
		String key;
#pragma region NfpExistChecker
		// 检查 Inner Fit Polygon (bin_nfp)
		key = getNfpKey(curr_piece);
		if (nfp_cache.find(key) == nfp_cache.end()) {
			std::cout << "Inner Fit Polygon doesn't exist: " + key << std::endl;
			continue;
		}
		polygon_t bin_nfp = nfp_cache.at(key);
		// 检查 curr_piece 与所有已放置的 placed_pieces 之间有 No Fit Polygon
		bool error = false;
		for (auto &each_placed_piece : placed_pieces) {
			key = getNfpKey(each_placed_piece, curr_piece);
			if (nfp_cache.find(key) == nfp_cache.end()) {
				error = true; 
				break;
			}
		}
		if (error) {
			std::cout << "No Fit Polygon doesn't exist: " + key << std::endl;
			continue; 
		}
#pragma endregion NfpExistChecker
			
		Vector curr_vector;
#pragma region FirstPiece
		// 放置第一块零件
		if (placed_pieces.empty()) {
			curr_vector.x = DistanceMax;
			auto & refer_point = curr_piece.poly.outer()[0]; // 参考点
			for (auto &nfp_point : bin_nfp.outer()) {
				// 寻找 nfp 最左边的位置，摆放坐标和原始坐标之间转换用矢量
				if (nfp_point.x() - refer_point.x() < curr_vector.x) {
					curr_vector = Vector(
						curr_piece.id,
						nfp_point.x() - refer_point.x(),
						nfp_point.y() - refer_point.y(),
						curr_piece.rotation
					);
				}
			}
			placed_pieces.push_back(curr_piece);
			placed_vectors.push_back(curr_vector);
			continue;
		}
#pragma endregion FirstPiece

#pragma region ClipperExecute
		Paths clipperUnionNfp;
		Paths clipperFinalNfp;
		Clipper clipperUnion;
		Clipper clipperDifference;

		/*
		* bin_nfp 转换成 clipper paths，即 clipperBinNfp.
		*/
		Paths clipperBinNfp = boost2ClipperPolygon(bin_nfp);

		/* 
		* nfp 转换成 clipper paths, 求并集得到 clipperUnionNfp.
		*/
		for (int j = 0; j < placed_pieces.size(); ++j) {
			key = getNfpKey(placed_pieces[j], curr_piece);
			Paths clipperNfp = boost2ClipperPolygon(nfp_cache.at(key));
			for (auto & each_path : clipperNfp) {
				for (auto & each_point : each_path) {
					each_point.X += static_cast<cInt>(placed_vectors[j].x * Config::scaleRate);
					each_point.Y += static_cast<cInt>(placed_vectors[j].y * Config::scaleRate);
				}
			}
			clipperUnion.AddPaths(clipperNfp, PolyType::ptSubject, true);
		}
		if (!clipperUnion.Execute(ClipType::ctUnion, clipperUnionNfp, PolyFillType::pftEvenOdd, PolyFillType::pftEvenOdd)) {
			std::cout << "clipperUnion Execute Failed: " << curr_piece.id << std::endl;
			continue;
		}

		/*
		* clipperBinNfp 经 clipperUnionNfp 裁剪（求差集）得到 clipperFinalNfp.
		*/
		clipperDifference.AddPaths(clipperBinNfp, PolyType::ptSubject, true);
		clipperDifference.AddPaths(clipperUnionNfp, PolyType::ptClip, true);
		if (!clipperDifference.Execute(ClipType::ctDifference, clipperFinalNfp, PolyFillType::pftEvenOdd, PolyFillType::pftEvenOdd))
		{
			std::cout << "clipperDifference Execute Failed: " << curr_piece.id << std::endl;
			continue;
		}

		/*
		* clean clipperFinalNfp
		*/
		CleanPolygons(clipperFinalNfp, 0.0001 * Config::scaleRate);
		clipperFinalNfp.erase(
			std::remove_if(clipperFinalNfp.begin(), clipperFinalNfp.end(), [](const Path &path) { 
				return path.size() < 3 || ClipperLib::Area(path) < 0.1 * Config::scaleRate * Config::scaleRate;
			}), 
			clipperFinalNfp.end()
		);
		if (clipperFinalNfp.empty()) {
			std::cout << "clipperFinalNfp is empty: " << curr_piece.id << std::endl;
			continue;
		}
#pragma endregion ClipperExecute

#pragma region Placement
		double min_eval_width = DistanceMax; // 评估当前放置方案的指标：宽度/面积/最右侧坐标
		double min_shift_X = DistanceMax;
			
		// placed_pieces 坐标偏移，记录全局矩形包络
		double left = bin.max_corner().x(),
				right = bin.min_corner().x(),
				bottom = bin.max_corner().y(),
				top = bin.min_corner().y();
		for (int j = 0; j < placed_pieces.size(); ++j) {
			polygon_t j_poly;
			for (auto & each_point : placed_pieces[j].poly.outer()) {
				bg::append(j_poly.outer(), point_t(each_point.x() + placed_vectors[j].x,
											        each_point.y() + placed_vectors[j].y));
			}
			box_t envelope;
			getEnvelope(j_poly, envelope);
			left = envelope.min_corner().x() < left ? envelope.min_corner().x() : left;
			right = envelope.max_corner().x() > right ? envelope.max_corner().x() : right;
			bottom = envelope.min_corner().y() < bottom ? envelope.min_corner().y() : bottom;
			top = envelope.max_corner().y() > top ? envelope.max_corner().y() : top;
		}

		// 在 final_nfp 的每个顶点上放置零件
		List<ring_t> final_nfp; 
		final_nfp.reserve(clipperFinalNfp.size());
		for (auto & each_path : clipperFinalNfp) {
			final_nfp.push_back(clipper2BoostRing(each_path));
		}
		for (auto & nfp_ring : final_nfp) {
			auto & refer_point = curr_piece.poly.outer()[0]; // 参考点
			for (auto & nfp_point : nfp_ring) {
				Vector shift_vector(
					curr_piece.id,
					nfp_point.x() - refer_point.x(),
					nfp_point.y() - refer_point.y(),
					curr_piece.rotation
				);

				// curr_piece 坐标偏移，更新矩形包络
				polygon_t shift_piece;
				for (auto & each_point : curr_piece.poly.outer()) {
					bg::append(shift_piece.outer(), point_t(each_point.x() + shift_vector.x,
													        each_point.y() + shift_vector.y));
				}
				box_t envelope;
				getEnvelope(shift_piece, envelope);
				left = envelope.min_corner().x() < left ? envelope.min_corner().x() : left;
				right = envelope.max_corner().x() > right ? envelope.max_corner().x() : right;
				bottom = envelope.min_corner().y() < bottom ? envelope.min_corner().y() : bottom;
				top = envelope.max_corner().y() > top ? envelope.max_corner().y() : top;
				auto eval_width = right - left;
				if (!almostEqual(eval_width, min_eval_width) && eval_width < min_eval_width
					|| almostEqual(eval_width, min_eval_width) && shift_vector.x < min_shift_X) {
					// 评价值更新
					min_eval_width = eval_width;
					min_shift_X = shift_vector.x;
					curr_vector = shift_vector;
					// 目标值更新
					min_obj_width = min_eval_width;
				}
			}
		}
		placed_pieces.push_back(curr_piece);
		placed_vectors.push_back(curr_vector);
#pragma endregion Placement
	}

	fitness += min_obj_width / (bin.max_corner().x() - bin.min_corner().x());
	// 如果在一块面料上完成排料，candidate_pieces 会被清空
	for (auto & placed_piece : placed_pieces) {
		candidate_pieces.erase(std::find(candidate_pieces.begin(), candidate_pieces.end(), placed_piece));
	}

	fitness += candidate_pieces.size(); // 存在空面料放不下的零件（不合法零件）
	return fitness;
}

void Solver::saveOutput() {
	for (auto & each_piece : pieces) {
		auto &each_item = input.items[pieceIdMap[each_piece.id]];
		each_item.res_coords.clear();
		each_item.res_coords.reserve(each_item.raw_coords.size());
		for (auto &each_xy : each_item.raw_coords) {
			// 初赛暂时只考虑 0 和 180 旋转
			if (each_piece.rotation == 180) {
				each_item.res_coords.push_back(
					{ each_piece.offsetX - each_xy.x, each_piece.offsetY - each_xy.y }
				);
			}
			else {
				each_item.res_coords.push_back(
					{ each_piece.offsetX + each_xy.x, each_piece.offsetY + each_xy.y }
				);
			}
		}
	}
}

}
