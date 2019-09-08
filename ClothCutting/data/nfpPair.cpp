#include "nfpPair.h"
#include "../utils/myUtils.hpp"

namespace cloth_cutting {

/*
* 生成 Piece A 和 B 的 NFP
* 暂时不考虑内环，考虑凹多边形
*/
void NfpPair::nfpPairGenerator(){
	Piece rA = A, rB = B;
	rotatePolygon(A.poly, rA.poly, rA.rotation);
	rotatePolygon(B.poly, rB.poly, rB.rotation);

	List<Piece> nfp;
	if (type == Type::Adjacent) {
		int count = 0;
		if (searchEdges) {
			nfp = GeometryUtil.noFitPolygon(A, B, false, searchEdges);
			if (nfp == null) {

			}
		}
		else {

			nfp = GeometryUtil.minkowskiDifference(A, B);
		}
		// sanity check
		if (nfp == null || nfp.size() == 0) {

			return null;
		}
		for (int i = 0; i < nfp.size(); i++) {
			if (!searchEdges || i == 0) {
				if (Math.abs(GeometryUtil.polygonArea(nfp.get(i))) < Math.abs(GeometryUtil.polygonArea(A))) {
					nfp.remove(i);

					return null;
				}
			}
		}
		if (nfp.size() == 0) {

			return null;
		}

		for (int i = 0; i < nfp.size(); i++) {
			if (GeometryUtil.polygonArea(nfp.get(i)) > 0) {
				nfp.get(i).reverse();
			}

			if (i > 0) {
				if (GeometryUtil.pointInPolygon(nfp.get(i).get(0), nfp.get(0))) {
					if (GeometryUtil.polygonArea(nfp.get(i)) < 0) {
						nfp.get(i).reverse();
					}
				}
			}
		}
	}
	return new ParallelData(pair.getKey(), nfp);
}

/*
* 构造 NFP
*/
void NfpPair::noFitPolygon(polygon_t &res_poly) {
	auto & rA = A.poly.outer();
	auto & rB = B.poly.outer();

	// 找到 A 左下角的点，B 最上方的点
	auto minAY = rA[0].y();
	auto minAX = rA[0].x();
	auto maxBY = rB[0].y();
	auto maxBX = rB[0].x();
	for (int i = 1; i < rA.size(); ++i) {
		if (rA[i].y() < minAY) {
			minAY = rA[i].y();
			minAX = rA[i].x();
		}
		else if(rA[i].y() == minAY){
			if (rA[i].x() < minAX) {
				minAX = rA[i].x();
			}
		}
	}
	for (int i = 1; i < rB.size(); ++i) {
		if (rB[i].y() > maxBY) {
			maxBY = rB[i].y();
			maxBX = rB[i].x();
		}
	}
	
	point_t start_point;
	if (type != Type::Inside) {
		start_point.x(minAX - maxBX);
		start_point.y(minAY - maxBY);
	}
	else {
		start_point = searchStartPoint(A, B, true, null);
	}

	List<polygon_t> NFPList;
	while (!bg::equals(start_point, invalidPoint)) {
		point_t prev_vector(invalidPoint);
		B.offsetX = start_point.x();
		B.offsetY = start_point.y();
		polygon_t NFP;
		bg::append(NFP.outer(), point_t(rB[0].x() + B.offsetX, rB[0].y() + B.offsetY));
		List<polygonRelation> relations;
		int count = 0;
		while (count < 10 * (rA.size() + rB.size())) {
			for (int i = 0; i < rA.size() - 1; ++i) { // 起点 = 终点
				for (int j = 0; j < rB.size() - 1; ++j) {
					point_t &Ai = rA[i], &Aii = rA[i + 1];
					point_t Bj(rB[j].x() + B.offsetX, rB[j].y() + B.offsetY);
					point_t Bjj(rB[j + 1].x() + B.offsetX, rB[j + 1].y() + B.offsetY);
					if (bg::equals(Ai, Bj)) {
						relations.emplace_back(i, j, Relation::Equals);
					}
					else if (bg::intersects(segment_t(Ai, Aii), Bj)) {
						relations.emplace_back(i + 1, j, Relation::BOnSegmentA);
					}
					else if (bg::intersects(segment_t(Bj, Bjj), Ai)) {
						relations.emplace_back(i, j + 1, Relation::AOnSegmentB);
					}
				}
			}

			polygon_t vectors;
			List<bool> isMarkedA(rA.size() - 1, false);
			List<bool> isMarkedB(rB.size() - 1, false);
			for (int i = 0; i < relations.size(); ++i) {
				ID currAIndex = relations[i].iA;
				ID prevAIndex = currAIndex == 0 ? rA.size() - 2 : currAIndex - 1;
				ID nextAIndex = currAIndex + 1;
				ID currBIndex = relations[i].iB;
				ID prevBIndex = currBIndex == 0 ? rB.size() - 2 : currBIndex - 1;
				ID nextBIndex = currBIndex + 1;

				point_t currA = rA[currAIndex];
				point_t prevA = rA[prevAIndex];
				point_t nextA = rA[nextAIndex];
				point_t currB = rB[currBIndex];
				point_t prevB = rB[prevBIndex];
				point_t nextB = rB[nextBIndex];

				if (relations[i].relation == Relation::Equals) {
					point_t vA1(prevA.x() - currA.x(), prevA.y() - currA.y());
					point_t vA2(nextA.x() - currA.x(), nextA.y() - currA.y());
					point_t vB1(currB.x() - prevB.x(), currB.y() - prevB.y());
					point_t vB2(currB.y() - nextB.x(), currB.y() - nextB.y());
					bg::append(vectors.outer(), vA1);
					bg::append(vectors.outer(), vA2);
					bg::append(vectors.outer(), vB1);
					bg::append(vectors.outer(), vB2);
					bg::append(vectors.outer(), vA1);
				}
				else if (relations[i].relation == Relation::BOnSegmentA) {
					point_t v1(currA.x() - (currB.x() + B.offsetX), currA.y() - (currB.y() + B.offsetY));
					point_t v2(prevA.x() - (currB.x() + B.offsetX), prevA.y() - (currB.y() + B.offsetY));
					bg::append(vectors.outer(), v1);
					bg::append(vectors.outer(), v2);
				}
				else if (relations[i].relation == Relation::AOnSegmentB) {
					point_t v1(currA.x() - (currB.x() + B.offsetX), currA.y() - (currB.y() + B.offsetY));
					point_t v2(currA.x() - (prevB.x() + B.offsetX), currA.y() - (prevB.y() + B.offsetY));
					bg::append(vectors.outer(), v1);
					bg::append(vectors.outer(), v2);
				}
			}

			point_t translate(invalidPoint);
			double maxd = 0.0;
			for (auto iter = vectors.outer().begin(); iter != vectors.outer().end(); ++iter) {
				if ((*iter).x() == 0 && (*iter).y() == 0) {
					continue;
				}

				if (!bg::equals(prev_vector, invalidPoint) && (*iter).y() * prev_vector.y() + (*iter).x() * prev_vector.x() < 0) {
					double vector_length = sqrt((*iter).x() * (*iter).x() + (*iter).y() * (*iter).y());
					point_t vector_unit((*iter).x() / vector_length, (*iter).y() / vector_length);

					double prev_length = sqrt(prev_vector.x() * prev_vector.x() + prev_vector.y() * prev_vector.y());
					point_t prev_unit(prev_vector.x() / prev_length, prev_vector.y() / prev_length);

					if (abs(vector_unit.y() * prev_unit.x() - vector_unit.x() * prev_unit.y()) < 0.0001) {
						continue;
					}
				}

				// Determination of Sliding Distance
				Distance d = polygonSlideDistance(A, B, (*iter), true);
				double vecd2 = (*iter).x() * (*iter).x() + (*iter).y() * (*iter).y();
				if (d == 0 || d * d > vecd2) {
					d = sqrt(vecd2);
				}
				if (d != 0 && d > maxd) {
					maxd = d;
					translate = (*iter);
				}
			}

			if (bg::equals(translate, invalidPoint) || almostEqual(maxd, 0)) {
				NFP = null;
				break;
			}

			translate.start.marked = true;
			translate.end.marked = true;
			prev_vector = translate;

			// trim
			double vlength2 = translate.x() * translate.x() + translate.y() * translate.y();
			if (maxd * maxd < vlength2 && !almostEqual(maxd * maxd, vlength2)) {
				double scale = sqrt((maxd*maxd) / vlength2);
				translate.x() *= scale;
				translate.y() *= scale;
			}

			referenceX += translate.x;
			referenceY += translate.y;


			if (almostEqual(referenceX, startX) && almostEqual(referenceY, startY)) {
				// we've made a full loop
				break;
			}
			
			// if A and B start on a touching horizontal line, the end point may not be the start point
			boolean looped = false;
			if (NFP.size() > 0) {
				for (int i = 0; i < NFP.size() - 1; i++) {
					if (almostEqual(referenceX, NFP.get(i).x) && almostEqual(referenceY, NFP.get(i).y)) {
						looped = true;
					}
				}
			}

			if (looped) {
				// we've made a full loop
				break;
			}

			NFP.add(new Segment(referenceX, referenceY));

			B.offsetX += translate.x;
			B.offsetY += translate.y;
			count++;
		}

		if (NFP != null && NFP.size() > 0) {
			NFPlist.add(NFP);
		}

		if (!searchEdges) {
			// only get outer NFP or first inner NFP
			break;
		}
		startPoint = searchStartPoint(A, B, inside, NFPlist);
	}
	return NFPlist;
}

}