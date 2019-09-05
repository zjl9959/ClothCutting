////////////////////////////////
/// usage : 1.	common type aliases.
/// 
/// note  : 1.	
////////////////////////////////

#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include <set>
#include <map>
#include <string>
#include <unordered_set>
#include <unordered_map>

namespace cloth_cutting {

// zero-based consecutive integer identifier.
using ID = int;
// the unit of x and y coordinates.
using Coord = double;
// 题目给出的面料长宽、瑕疵半径、间距等属性为整型
using Length = int;
// 几何运算产生的结果为浮点型
using Distance = double;
// angle
using Angle = int;
// the unit of elapsed computational time.
using Duration = int;
// number of neighborhood moves in local search.
using Iteration = int;

template<typename T>
using List = std::vector<T>;

template<typename Key>
using HashSet = std::unordered_set<Key>;

template<typename T>
using Set = std::set<T>;

template<typename  Key, typename Val>
using HashMap = std::unordered_map<Key, Val>;

template<typename Key, typename Val>
using Map = std::map<Key, Val>;

using String = std::string;

enum 
{
	MaxDistance = (1 << 28)
};

enum 
{
	InvalidID = -1
};

class FileExtension {
public:
    static String protobuf() { return String(".pb"); }
    static String json() { return String(".json"); }
};

}


#endif // COMMON_H
