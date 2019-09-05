#include "piece.h"
#include <cassert>

namespace cloth_cutting {

/*
* 删除共线（或几乎共线）和半邻接的顶点
* 暂时只考虑外环
*/
void Piece::cleanPiece(Piece &res_piece) const {
	Path in_poly = boost2ClipperRing(poly.outer());
	Paths out_polys;
	// 简化多边形
	SimplifyPolygon(in_poly, out_polys, PolyFillType::pftNonZero);
	if (out_polys.empty()) { return; }
	
	Path biggest_poly = out_polys[0];
	double biggest_area = ClipperLib::Area(biggest_poly);
	for (auto &each_poly : out_polys) {
		double each_area = ClipperLib::Area(each_poly);
		biggest_poly = each_area > biggest_area ? each_poly : biggest_poly;
		biggest_area = each_area > biggest_area ? each_area : biggest_area;
	}
	// 删除顶点
	CleanPolygon(biggest_poly, Config::curveTolerance * Config::scaleRate);
	if (biggest_poly.empty()) { return; }

	res_piece.id = this->id;
	res_piece.rotation = this->rotation;
	res_piece.offsetX = this->offsetX;
	res_piece.offsetY = this->offsetY;
	res_piece.poly.outer() = clipper2BoostRing(biggest_poly);
}

/*
* 缩放多边形
* offset < 0，一个多边形可能会被拆分成多个小多边形
* offset > 0，向外膨胀，只有一个多边形，暂时只考虑这一种情况
*/
void Piece::offsetPiece(List<Piece>& res_pieces, double offset) const {
	if (offset == 0) { return; }
	Path in_poly = boost2ClipperRing(poly.outer());
	Paths out_polys;
	// JoinType = jtMiter，MiterLimit作用；JoinType = jtRound，ArcTolerance作用。
	// JoinType = jtRound，使用弧线包裹尖角
	ClipperOffset co(2.0, Config::curveTolerance * Config::scaleRate);
	co.AddPath(in_poly, JoinType::jtRound, EndType::etClosedPolygon);
	co.Execute(out_polys, offset * Config::scaleRate);

	res_pieces.resize(out_polys.size());
	for (int i = 0; i < out_polys.size(); ++i) {
		res_pieces[i].id = this->id;
		res_pieces[i].rotation = this->rotation;
		res_pieces[i].offsetX = this->offsetX;
		res_pieces[i].offsetY = this->offsetY;
		res_pieces[i].poly.outer() = clipper2BoostRing(out_polys[i]);
	}
	if (offset > 0) { assert(res_pieces.size() == 1); }
}

}