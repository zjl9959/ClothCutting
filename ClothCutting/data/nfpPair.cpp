#include "nfpPair.h"
#include <libnfporb.hpp>

namespace cloth_cutting {
	namespace libnfp = libnfporb;
	
	// 点
	static libnfp::point_t boost2NfpPoint(const point_t &point) {
		if (typeid(libnfp::coord_t) == typeid(point.x())) { return libnfp::point_t(point.x(), point.y()); }
		return libnfp::point_t(static_cast<libnfp::coord_t>(point.x()), static_cast<libnfp::coord_t>(point.y()));
	}

	static point_t nfp2BoostPoint(const libnfp::point_t &point) {
		if (typeid(Coord) == typeid(point.x_.val())) { return point_t(point.x_.val(), point.y_.val()); }
		return point_t(static_cast<Coord>(point.x_.val()), static_cast<Coord>(point.y_.val()));
	}

	// 环
	static libnfp::polygon_t::ring_type boost2NpfRing(const ring_t &ring) {
		libnfp::polygon_t::ring_type res_ring;
		for (auto iter = ring.begin(); iter != ring.end(); ++iter) { bg::append(res_ring, boost2NfpPoint(*iter)); }
		return res_ring;
	}

	static ring_t nfp2BoostRing(const libnfp::polygon_t::ring_type &ring) {
		ring_t res_ring;
		for (auto iter = ring.begin(); iter != ring.end(); ++iter) { bg::append(res_ring, nfp2BoostPoint(*iter)); }
		return res_ring;
	}

	// 多边形
	static libnfp::polygon_t boost2NfpPolygon(const polygon_t &poly) {
		libnfp::polygon_t res_poly;
		res_poly.outer() = boost2NpfRing(poly.outer());
		for (const auto &inner : poly.inners()) { res_poly.inners().push_back(boost2NpfRing(inner)); }
		return res_poly;
	}

	static polygon_t nfp2BoostPolygon(const libnfp::polygon_t &poly) {
		polygon_t res_poly;
		res_poly.outer() = nfp2BoostRing(poly.outer());
		for (const auto &inner : poly.inners()) { res_poly.inners().push_back(nfp2BoostRing(inner)); }
		return res_poly;
	}
	
	// 将 nfp 数组转换成多边形
	static polygon_t nfpRings2BoostPolygon(const libnfp::nfp_t &nfp) {
		// 无内环 nfp.size() == 1
		libnfp::polygon_t nfppoly;
		for (const auto& pt : nfp.front()) {
			nfppoly.outer().push_back(pt);
		}
		for (size_t i = 1; i < nfp.size(); ++i) {
			nfppoly.inners().push_back({});
			for (const auto &pt : nfp[i]) {
				nfppoly.inners().back().push_back(pt);
			}
		}
		return nfp2BoostPolygon(nfppoly);
	}

	/*
	* 生成 nfp
	*/
	void NfpPair::nfpPairGenerator() {
		libnfp::polygon_t pA = boost2NfpPolygon(A.poly);
		libnfp::polygon_t pB = boost2NfpPolygon(B.poly);
		libnfp::nfp_t nfp = libnfp::generateNFP(pA, pB);
		write_svg("nfp.svg", { pA, pB }, nfp);
		this->nfp = nfpRings2BoostPolygon(nfp);
	}

}
