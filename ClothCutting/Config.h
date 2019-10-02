#pragma once
#ifndef CLOTH_CUTTING_CONFIG_H
#define CLOTH_CUTTING_CONFIG_H

#include "Common.h"

namespace cloth_cutting {
	
struct Config {
	// 全局变量
	// double 转 clipper_cInt, 将坐标变为整数的最小缩放倍数，clipper 函数的调用都需要乘该倍数
	static constexpr int scaleRate = 10; 
	// 曲线宽恕，删除与边距离 curveTolerance 以内的坐标点，如果凸多边形会造成面积减少
	static constexpr double curveTolerance = 0.025; 

	Length minGap;     // 间距
	Length minPadding; // 边距

	Config(Length mg = 5, Length mp = 0) : minGap(mg), minPadding(mp) {}
};

}


#endif // !CLOTH_CUTTING_CONFIG_H
