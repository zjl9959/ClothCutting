#pragma once
#ifndef CLOTH_CUTTING_PROBLEM_H
#define CLOTH_CUTTING_PROBLEM_H

#include <fstream>
#include <sstream>
#include <iostream>
#include "../Common.h"

namespace cloth_cutting {

struct Coordinate {
	Coord x, y;
};

// 零件
class Item {
public:
	String batchIndex; // 批次号
	String itemIndex; // 零件号 
	int amount; // 零件个数
	List<Coordinate> raw_coords; // 零件的原始轮廓坐标
	List<Coordinate> res_coords; // 结果中零件的轮廓坐标
	List<Angle> rotateAngles; // 可旋转的角度
	String plateIndex; // 面料号
};

// 瑕疵
class Defect {
public:
    Coordinate center; // 圆心
    Length radius; // 半径
};

// 面料
class Plate {
public:
	String plateIndex; // 面料号
	Length width, height; // 长和宽
	List<Defect> defects; // 面料的瑕疵
	Length minGap; // 零件间的最小间距
	Length minPadding; // 最小边距
};

// 输入
class Input {
public:
	List<Item> items; // 零件列表
	List<Plate> plates; // 面料列表
    
    Input();
    void readItems(std::ifstream &fin);
    void readPlates(std::ifstream &fin);
    String Trim(String& str);
};

// 输出
class Output {
public:
    void save(List<Item> &items);
};

}

#endif // !CLOTH_CUTTING_PROBLEM_H
