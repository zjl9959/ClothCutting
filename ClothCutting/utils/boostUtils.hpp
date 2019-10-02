#pragma once
#ifndef CLOTH_CUTTING_BOOST_UTILS_HPP
#define CLOTH_CUTTING_BOOST_UTILS_HPP

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/geometries.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include "../Common.h"

namespace cloth_cutting {

namespace bg = boost::geometry;

using T = Coord;

// n 维点坐标
template<size_t dimension = 2>
using point_base = bg::model::point<T, dimension, bg::cs::cartesian>;

/*****************************
* 以下定义全部针对二维点坐标 *
******************************/

// 二维坐标点
using point_t = bg::model::d2::point_xy<T>;
const point_t originPoint(0, 0);
const point_t invalidPoint(-1, -1);

// 曲线
using linestring_t = bg::model::linestring<point_t>;

// 多边形（逆时针，起点=终点，包括0个或者多个内环 inner rings）
using polygon_t = bg::model::polygon<point_t, false, true>;

// 环（逆时针，起点=终点）
//using ring_t = bg::model::ring<point_t, false, true>;
using ring_t = polygon_t::ring_type;

// 点集合
using multi_point_t = bg::model::multi_point<point_t>;

// 曲线集合
using multi_linestring_t = bg::model::multi_linestring<linestring_t>;

// 多边形集合
using multi_polygon_t = bg::model::multi_polygon<polygon_t>;

// 矩形
using box_t = bg::model::box<point_t>;

// 线段（坐标点对）
using segment_t = bg::model::segment<point_t>;

// 多边形平移
static void translatePolygon(const polygon_t &poly, polygon_t &translate_poly, T x, T y) {
	bg::strategy::transform::translate_transformer<double, 2, 2> translate_strategy(x, y);
	bg::transform(poly, translate_poly, translate_strategy);
}

// 多边形绕原点旋转一定角度
static void rotatePolygon(const polygon_t &poly, polygon_t &rotate_poly, Angle angle) {
	bg::strategy::transform::rotate_transformer<bg::degree, double, 2, 2> rotate_strategy(angle);
	bg::transform(poly, rotate_poly, rotate_strategy);
}

// 求多边形的包络矩形
using rectangle_t = std::pair<T, T>;
static rectangle_t getEnvelope(const polygon_t &poly, box_t &envelope) {
	bg::envelope(poly, envelope);
	return { 
		bg::get<bg::max_corner, 0>(envelope) - bg::get<bg::min_corner, 0>(envelope), // width
		bg::get<bg::max_corner, 1>(envelope) - bg::get<bg::min_corner, 1>(envelope)  // height
	};
}

}

#endif // !CLOTH_CUTTING_BOOST_UTILS_HPP