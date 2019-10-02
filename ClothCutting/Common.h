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
#include <limits>

namespace cloth_cutting {

// zero-based consecutive integer identifier.
using ID = int;
// 坐标类型（用于 boost_geometry, Clipper 坐标类型为 cInt）
using Coord = double;
const auto DistanceMax = (std::numeric_limits<Coord>::max)();
// 题目给出的面料长宽、瑕疵半径、间距等属性为整型
using Length = int;
// 旋转角度 [0, 90, 180, 270, 360]
using Angle = int;

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

}


#endif // COMMON_H
