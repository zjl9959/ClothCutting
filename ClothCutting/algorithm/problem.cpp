#include "problem.h"

namespace cloth_cutting {

Input::Input() {
    String instanceFolder = "./Instance/dataA/";
    std::ifstream finItems(instanceFolder + "L0002_lingjian.csv");
    std::ifstream finPlates(instanceFolder + "L0002_mianliao.csv");
    readItems(finItems);
    readPlates(finPlates);
}

void Input::readItems(std::ifstream & fin) {
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

void Input::readPlates(std::ifstream & fin) {
    String line;
    std::getline(fin, line); // 弃掉首行
    //while (std::getline(fin, line)) {
    line = line.substr(line.find('\r') + 1, line.size());
    Plate plate;
    std::istringstream sin(line);
    std::getline(sin, plate.plateIndex, ',');
    String temp;
    std::getline(sin, temp, ',');
    plate.width = atoi(temp.substr(0, temp.find('*')).c_str());
    plate.height = atoi(temp.substr(temp.find('*') + 1, temp.size()).c_str());

    std::getline(sin, temp, '\"');
    if (temp != "") {// 说明不存在瑕疵
        std::istringstream nextIn(temp);
        std::getline(nextIn, temp, ',');
        std::getline(nextIn, temp, ',');
        plate.minGap = atoi(temp.c_str());
        std::getline(nextIn, temp, ',');
        plate.minPadding = atoi(temp.c_str());
    } else {
        std::getline(sin, temp, '\"');
        std::istringstream defectsIn(temp);
        int cnt = 1;
        Defect defect;
        while (std::getline(defectsIn, temp, ',')) {
            if (cnt == 1) {
                defect.center.x = atof(Trim(temp).c_str());
            } else if (cnt == 2) {
                defect.center.y = atof((Trim(temp).c_str()));
            } else if (cnt == 3) {
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
}

String Input::Trim(String & str) {
    //str.find_first_not_of(" \t\r\n"),在字符串str中从索引0开始，返回首次不匹配"\t\r\n"的位置
    str.erase(0, str.find_first_not_of(", ["));
    str.erase(str.find_last_not_of("]") + 1);
    /*str.erase(0, str.find_first_not_of(" \t\r\n"));
    str.erase(str.find_last_not_of(" \t\r\n") + 1);*/
    return str;
}

void Output::save(List<Item>& items) {
    String solutionFolder = "./Solution/";
    std::ofstream ofs(solutionFolder + "dataA.csv");
	ofs << "下料批次号,零件号,面料号,零件外轮廓线坐标" << std::endl; // 表头
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

}

