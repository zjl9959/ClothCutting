# ClothCutting
天池2019广东工业智造创新大赛，布料切割方案算法设计



## 环境依赖

- Microsoft Visual Studio 2017
- [Boost Geometry 1.65.1](http://www.boost.org/doc/libs/1_65_1/libs/geometry/doc/html/index.html)：header-only-library 无需编译，需配置环境变量`BoostPath = path\to\boost_1_65_1`
- [Clipper](http://www.angusj.com/delphi/clipper.php)：附加包含目录已追踪
- [libnfporb](https://github.com/martinhansdk/libnfporb)：用于生成 nfp 的 header-only-library，附加包含目录已追踪
- Cmake



## TODO

- 检查算例中零件方向，统一为按照逆时针方向旋转；
- 使用 clipper 放大、简化多边形的顺序和必要性？
- libnfporb 实现有问题？
- 添加瑕疵，将瑕疵转换为多边形，同时也要参加差集运算；
- 改写为 cmake 工程，适配 Linux 平台，最终打包成 docker 镜像。



## 参考

Nest4J：https://github.com/Yisaer/Nest4J

libnfporb 参考论文：[E.K. Burke *et al.* 2006](http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.440.379&rep=rep1&type=pdf)

基于可行解（贪心、模拟退火）：[Solving Irregular Strip Packing problems by hybridising simulated annealing and linear programming](https://www.sciencedirect.com/science/article/pii/S0377221704005879?via%3Dihub)

基于不可行解（重叠消除）：[An iterated local search algorithm based on nonlinear programming for the irregular strip packing problem](https://www.sciencedirect.com/science/article/pii/S1572528609000218?via%3Dihub)

Ps：目前实现的是基于可行解的贪心放置策略，几何运算开销较大，理论上基于不可行解的重叠消除效果可能更好。

