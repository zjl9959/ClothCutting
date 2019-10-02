#pragma once
#ifndef CLOTH_CUTTING_CLIPPER_UTILS_HPP
#define CLOTH_CUTTING_CLIPPER_UTILS_HPP

#include <clipper.hpp>
#include "boostUtils.hpp"
#include "../Config.h"

namespace cloth_cutting {

using namespace ClipperLib;

// 点
static point_t clipper2BoostPoint(const IntPoint &point) {
	if (typeid(Coord) == typeid(point.X)) { return point_t(point.X, point.Y); }
	return point_t(static_cast<Coord>(point.X / Config::scaleRate), static_cast<Coord>(point.Y / Config::scaleRate));
}

static IntPoint boost2ClipperPoint(const point_t &point) {
	if (typeid(cInt) == typeid(point.x())) { return IntPoint(point.x(), point.y()); }
	return IntPoint(static_cast<cInt>(point.x() * Config::scaleRate), static_cast<cInt>(point.y() * Config::scaleRate));
}

// 线
static linestring_t clipper2BoostLine(const Path &path) {
	linestring_t res_path;
	for (auto iter = path.begin(); iter != path.end(); ++iter) { bg::append(res_path, clipper2BoostPoint(*iter)); }
	return res_path;
}

static Path boost2ClipperLine(const linestring_t &path) {
	Path res_path;
	for (auto iter = path.begin(); iter != path.end(); ++iter) { res_path << boost2ClipperPoint(*iter); }
	return res_path;
}

// 环（起点 = 终点）
static ring_t clipper2BoostRing(const Path &path) {
	ring_t res_path;
	for (auto iter = path.begin(); iter != path.end(); ++iter) { bg::append(res_path, clipper2BoostPoint(*iter)); }
	bg::append(res_path, clipper2BoostPoint(path[0]));
	return res_path;
}

static Path boost2ClipperRing(const ring_t &path) {
	Path res_path;
	for (auto iter = path.begin(); iter != path.end() - 1; ++iter) { res_path << boost2ClipperPoint(*iter); }
	return res_path;
}

// 多边形
static polygon_t clipper2BoostPolygon(const Paths &poly) {
	polygon_t res_poly;
	// 面积最大的 Path 作为外环
	int biggest_index = 0;
	double biggest_area = ClipperLib::Area(poly[0]);
	for (int i = 1; i < poly.size(); ++i) {
		double i_area = ClipperLib::Area(poly[i]);
		biggest_index = i_area > biggest_area ? i : biggest_index;
		biggest_area = i_area > biggest_area ? i_area : biggest_area;
	}
	res_poly.outer() = clipper2BoostRing(poly[biggest_index]);  
	res_poly.inners().reserve(poly.size() - 1);
	for (int i = 0; i < poly.size() && i != biggest_index; ++i) {
		res_poly.inners().push_back(clipper2BoostRing(poly[i]));
	}
	return res_poly;
}

static Paths boost2ClipperPolygon(const polygon_t &poly) {
	Paths res_paths; 
	res_paths.reserve(poly.inners().size() + 1);
	res_paths.push_back(boost2ClipperRing(poly.outer()));
	for (auto & inner_ring : poly.inners()) {
		res_paths.push_back(boost2ClipperRing(inner_ring));
	}
	return res_paths;
}

}



#endif // !CLOTH_CUTTING_CLIPPER_UTILS_HPP
