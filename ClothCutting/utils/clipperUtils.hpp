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

}



#endif // !CLOTH_CUTTING_CLIPPER_UTILS_HPP
