#pragma once
#ifndef CLOTH_CUTTING_BOOST_UTILS_HPP
#define CLOTH_CUTTING_BOOST_UTILS_HPP

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/geometries.hpp>
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

// 点集合
using multi_point_t = bg::model::multi_point<point_t>;

// 曲线集合
using multi_linestring_t = bg::model::multi_linestring<linestring_t>;

// 多边形集合
using multi_polygon_t = bg::model::multi_polygon<polygon_t>;

// 矩形
using box_t = bg::model::box<point_t>;

// 环（逆时针，起点=终点）
using ring_t = bg::model::ring<point_t, false, true>;

// 线段（坐标点对）
using segment_t = bg::model::segment<point_t>;

// 旋转多边形一定角度
static void rotatePolygon(const polygon_t &poly, polygon_t &rotate_poly, Angle angle) {
	bg::strategy::transform::rotate_transformer<bg::degree, int, 2, 2> rotate_strategy(angle);
	bg::transform(poly, rotate_poly, rotate_strategy);
}

}

#endif // !CLOTH_CUTTING_BOOST_UTILS_HPP
