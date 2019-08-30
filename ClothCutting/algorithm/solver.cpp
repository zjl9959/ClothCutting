#include "Solver.h"

namespace cloth_cutting {
	
void Solver::run(){
	preprocess();
}

void Solver::preprocess() {
	// plate -> bin
	bins.reserve(input.plates.size());
	for (auto each_plate : input.plates) {
		bins.emplace_back(originPoint, point_t(each_plate.length, each_plate.width));
	}

	// item -> piece，平移多边形包络左下角到坐标原点处，减少坐标数值
	pieces.reserve(input.items.size() * 2);
	for (auto each_item : input.items) {
		int amount = each_item.amount;
		while (amount) {
			polygon_t piece;
			for (auto each_xy : each_item.raw_coords) {
				bg::append(piece.outer(), point_t(each_xy.x, each_xy.y));
			}
			box_t envelope = bg::return_envelope<box_t>(piece);
			auto offsetX = bg::get<bg::min_corner, 0>(envelope);
			auto offsetY = bg::get<bg::min_corner, 1>(envelope);
			//auto offsetX2 = envelope.min_corner().x();
			//auto offsetY2 = envelope.min_corner().y();
			piece.clear();
			for (auto each_xy : each_item.raw_coords) {
				bg::append(piece.outer(), point_t(each_xy.x - offsetX, each_xy.y - offsetY));
			}
			pieces.push_back(piece);
			--amount;
		}
	}
}


}
