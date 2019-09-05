#pragma once
#ifndef CLOTH_CUTTING_CONFIG_H
#define CLOTH_CUTTING_CONFIG_H

#include "Common.h"

namespace cloth_cutting {
	
struct Config {
	// 全局变量
	static constexpr int scaleRate = 10; // 缩放倍数
	static constexpr double curveTolerance = 0.02; // 曲线误差（暂不考虑误差）

	Length minGap;       // 间距
	Length minPadding;   // 边距
	int populationSize;  // 种群代数
	double mutationRate; // 突变率

	Config(Length mg = 5, Length mp = 0, int ps = 10, double mr = 0.1) :
		minGap(mg), minPadding(mp), populationSize(ps), mutationRate(mr) {}
};

}


#endif // !CLOTH_CUTTING_CONFIG_H
