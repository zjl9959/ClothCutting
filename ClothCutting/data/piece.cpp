#include "piece.h"
#include <cassert>

namespace cloth_cutting {
	/*
	* 将多边形平移到坐标原点
	*/
	void Piece::zeroPiece(Piece & res_piece) const {
		box_t envelope;
		getEnvelope(this->poly, envelope);
		auto moveX = 0 - envelope.min_corner().x();
		auto moveY = 0 - envelope.min_corner().y();
		translatePolygon(this->poly, res_piece.poly, moveX, moveY);
		res_piece.id = this->id;
		res_piece.rotation = this->rotation;
		res_piece.offsetX = this->offsetX;
		res_piece.offsetY = this->offsetY;
	}

	/*
	* 简化多边形：删除自相交、共线（或几乎共线）和半邻接的顶点
	*/
	void Piece::cleanPiece(Piece &res_piece) const {
		Paths polys = boost2ClipperPolygon(this->poly);

		// 删除自相交
		SimplifyPolygons(polys, PolyFillType::pftEvenOdd);

		// 删除共线顶点
		CleanPolygons(polys, Config::curveTolerance * Config::scaleRate);
		
		assert(polys.size() == 1);

		res_piece.id = this->id;
		res_piece.rotation = this->rotation;
		res_piece.offsetX = this->offsetX;
		res_piece.offsetY = this->offsetY;
		res_piece.poly = clipper2BoostPolygon(polys);
	}

	/*
	* 缩放多边形
	* offset > 0，向外膨胀确定只会产生一个多边形
	* offset < 0，一个多边形可能会被拆分成多个小多边形，不支持
	*/
	void Piece::offsetPiece(Piece &res_piece, double offset) const {
		if (offset <= 0) { 
			res_piece.poly = this->poly;
		}
		else {
			Paths in_polys = boost2ClipperPolygon(this->poly);
			Paths out_polys;
			// JoinType = jtMiter，超出倍数截断尖角，MiterLimit = 2.0
			// JoinType = jtRound，使用弧线包裹尖角, ArcTolerance = Config::curveTolerance * Config::scaleRate
			ClipperOffset co(2.0, Config::curveTolerance * Config::scaleRate);
			co.AddPaths(in_polys, JoinType::jtRound, EndType::etClosedPolygon);
			co.Execute(out_polys, offset * Config::scaleRate);
			assert(out_polys.size() == 1);
			res_piece.poly = clipper2BoostPolygon(out_polys);
		}
		res_piece.id = this->id;
		res_piece.rotation = this->rotation;
		res_piece.offsetX = this->offsetX;
		res_piece.offsetY = this->offsetY;
	}

}