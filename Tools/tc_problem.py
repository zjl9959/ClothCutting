#!usr/bin/env python3

'''
该脚本实现数据导入功能，被其它脚本调用
1. 实现对面料数据的定义
2. 实现对零件数据的定义
3. 实现对结果数据的定义
4. 实现数据的加载
'''

import csv


DEPLOY_DIR = '../Deploy/'
SOLUTION_DIR = DEPLOY_DIR + 'Solution/'
INSTANCE_DIR = DEPLOY_DIR + 'Instance/'


class Coord:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __str__(self):
        return "[{},{}]".format(self.x, self.y)


class Item:
    def __init__(self, batchIndex, itemIndex, amount, coords, angles, plateIndex):
        self.batchIndex = batchIndex
        self.itemIndex = itemIndex
        self.amount = amount
        self.coords = coords
        self.angles = angles
        self.palteIndex = plateIndex


class Defect:
    def __init__(self, center, radius):
        self.center = center
        self.radius = radius

    def __str__(self):
        return "[{},{}]".format(self.center, self.radius)


class Plate:
    def __init__(self, plateIndex, length, width, defects, minGap, minPadding):
        self.plateIndex = plateIndex
        self.length = length
        self.width = width
        self.defects = defects
        self.minGap = minGap
        self.minPadding = minPadding


class SolNode:
    def __init__(self, itemIndex, plateIndex, coords):
        self.itemIndex = itemIndex
        self.plateIndex = plateIndex
        self.coords = coords


def parseCoords(s):
    '''
    将输入的字符串解析成坐标
    输入：str()
    输入：list(Coord())
    '''
    coords = list()
    for scoord in s.strip('[').strip(']').split('],'):
        coord = scoord.strip(' ').strip('[').split(',')
        coords.append(Coord(float(coord[0]), float(coord[1])))
    return coords



def loadItems(file_path):
    '''
    加载算例中零件数据
    返回值：list(Item())
    '''
    items = list()
    with open(file_path, encoding = 'utf-8') as file:
        csv_reader = csv.reader(file)
        csv_head = next(csv_reader)
        for row in csv_reader:
            coords = parseCoords(row[3])
            angles = [int(i) for i in row[4].split(',')]
            item = Item(row[0], row[1], int(row[2]), coords, angles, row[5])
            items.append(item)
    return items


def loadPlates(file_path):
    '''
    加载算例中原料数据
    返回值：list(Plate())
    '''
    plates = list()
    with open(file_path, encoding = 'utf-8') as file:
        csv_reader = csv.reader(file)
        csv_head = next(csv_reader)
        for row in csv_reader:
            defects = list()
            for d in row[2].strip('[').strip(']').split('[['):
                if not d:
                    continue
                defect = d.replace(']', '').split(',')
                defects.append(Defect(Coord(float(defect[0]), float(defect[1])), float(defect[2])))
            plate_size = row[1].split('*')
            plates.append(Plate(row[0], plate_size[0], plate_size[1], defects, int(row[3]), int(row[4])))
    return plates


def loadSolution(file_path):
    '''
    加载输出解中的数据
    返回值：list(SolNode())
    '''
    solution = list()
    with open(file_path, encoding = 'utf-8') as file:
        csv_reader = csv.reader(file)
        csv_head = next(csv_reader)
        for row in csv_reader:
            solution.append(row[0], row[1], parseCoords(row[2]))
    return solution


def test_dataA():
    items_1 = loadItems(INSTANCE_DIR + 'dataA/L0002_lingjian.csv')
    print('dataA/L0002_lingjian.csv中零件数目：', len(items_1))
    plates_1 = loadPlates(INSTANCE_DIR + 'dataA/L0002_mianliao.csv')
    print('dataA/L0002_mianliao.csv中零件数目：', len(plates_1))
    items_2 = loadItems(INSTANCE_DIR + 'dataA/L0003_lingjian.csv')
    print('dataA/L0003_lingjian.csv中零件数目：', len(items_2))
    plates_2 = loadPlates(INSTANCE_DIR + 'dataA/L0003_mianliao.csv')
    print('dataA/L0003_mianliao.csv中零件数目：', len(plates_2))


if __name__ == '__main__':
    test_dataA()
