#pragma once
#ifndef CLOTH_CUTTING_PROBLEM_H
#define CLOTH_CUTTING_PROBLEM_H
#include<fstream>
#include<sstream>
#include<iostream>
#include"../Common.h"
namespace cloth_cutting {
	typedef class Defect Defect;
struct Coordinate {
public:
	double x, y;
};

// 零件
class Item {
public:
	String batchIndex; // 批次号
	String itemIndex; // 零件号 
	int amount; // 零件个数
	List<Coordinate> raw_coords; // 零件的原始轮廓坐标
	List<Coordinate> res_coords; // 结果中零件的轮廓坐标
	List<int> rotateAngles; // 可旋转的角度
	String plateIndex; // 面料号
};

// 面料
class Plate {
public:
	String plateIndex; // 面料号
	int length, width; // 长和宽
	List<Defect> defects; // 面料的瑕疵
	int minGap; // 零件间的最小间距
	int minPadding; // 最小边距
};

// 瑕疵
class Defect {
public:
	Coordinate center; // 圆心
	int radius; // 半径
};

// 输入
class Input {
public:
	List<Item> items; // 零件列表
	List<Plate> plates; // 面料列表
	Input() {
		String instanceFolder = "./Instance/dataA/";
		std::ifstream finItems(instanceFolder + "L0002_lingjian.csv");
		std::ifstream finPlates(instanceFolder + "L0002_mianliao.csv");
		readItems(finItems);
		readPlates(finPlates);
		
	}

	void readItems(std::ifstream &fin) {
		String line;
		std::getline(fin, line); // 弃掉首行
		while (std::getline(fin, line)) {
			std::istringstream sin(line);
			Item item;
			String field;
			std::getline(sin, item.batchIndex, ',');
			std::getline(sin, item.itemIndex, ',');
			std::getline(sin, field, ',');
			item.amount = atoi(field.c_str());
			std::getline(sin, field, '\"');
			std::getline(sin, field, '\"');
			field.erase(0, 1);
			field.erase(field.size() - 1);

			std::istringstream coordsIn(field);
			String temp;
			while (std::getline(coordsIn, temp, ']')) {
				temp = Trim(temp);
				//std::cout << temp << std::endl;
				Coordinate coord;

				coord.x = atof(temp.substr(0, temp.find(',')).c_str());
				coord.y = atof(temp.substr(temp.find(' ') + 1, temp.size()).c_str());
				item.raw_coords.push_back(coord);
			}
			std::getline(sin, field, '\"');
			std::getline(sin, field, '\"');

			std::istringstream anglesIn(field);
			while (std::getline(anglesIn, temp, ',')) {
				item.rotateAngles.push_back(atoi(temp.c_str()));
			}
			std::getline(sin, field, ',');
			std::getline(sin, field, ',');
			item.plateIndex = field;
			items.push_back(item);
		}
	}

	void readPlates(std::ifstream &fin) {
		String line;
		std::getline(fin, line); // 弃掉首行
		//while (std::getline(fin, line)) {
		line = line.substr(line.find('\r') + 1, line.size());
			Plate plate;
			std::istringstream sin(line);
			std::getline(sin, plate.plateIndex, ',');
			String temp;
			std::getline(sin, temp, ',');
			plate.length = atoi(temp.substr(0, temp.find('*')).c_str());
			plate.width = atoi(temp.substr(temp.find('*') + 1, temp.size()).c_str());

			std::getline(sin, temp, '\"');
			if (temp != "") {// 说明不存在瑕疵
				std::istringstream nextIn(temp);
				std::getline(nextIn, temp, ',');
				std::getline(nextIn, temp, ',');
				plate.minGap = atoi(temp.c_str());
				std::getline(nextIn, temp, ',');
				plate.minPadding = atoi(temp.c_str());
			}
			else {
				std::getline(sin, temp, '\"');
				std::istringstream defectsIn(temp);
				int cnt=1;
				Defect defect;
				while (std::getline(defectsIn,temp,',')){
					if (cnt == 1) {
						defect.center.x = atof(Trim(temp).c_str());
					}
					else if (cnt == 2) {
						defect.center.y = atof((Trim(temp).c_str()));
					}
					else if (cnt == 3) {
						defect.radius = atoi(Trim(temp).c_str());
						plate.defects.push_back(defect);
						cnt = 0;
					}
					cnt++;
				}
				std::getline(sin, temp, ',');
				std::getline(sin, temp, ',');
				plate.minGap = atoi(temp.c_str());
				std::getline(sin, temp, ',');
				plate.minPadding = atoi(temp.c_str());
				

			}
			plates.push_back(plate);

			
		//}
		
	}
	String Trim(String& str)
	{
		//str.find_first_not_of(" \t\r\n"),在字符串str中从索引0开始，返回首次不匹配"\t\r\n"的位置
		str.erase(0, str.find_first_not_of(", ["));
		str.erase(str.find_last_not_of("]")+1);
		/*str.erase(0, str.find_first_not_of(" \t\r\n"));
		str.erase(str.find_last_not_of(" \t\r\n") + 1);*/
		return str;
	}
};

// 输出
class OutPut {
public:
	void save(List<Item> &items) {
		String solutionFolder = "./Solution/";
		std::ofstream ofs(solutionFolder + "dataA.csv");
		for (auto i = 0; i < items.size(); ++i) {
			ofs << items[i].batchIndex << "," << items[i].itemIndex << "," << items[i].plateIndex << ",";
			ofs << "\"[";
			for (auto j = 0; j < items[i].res_coords.size(); ++j) {
				ofs << "[" << items[i].res_coords[j].x << "," << items[i].res_coords[j].y << "]";
				if (j < items[i].res_coords.size() - 1) {
					ofs << ",";
				}
			}
			ofs << "\"]" << std::endl;
		}
	}
};

}

#endif // !CLOTH_CUTTING_PROBLEM_H
