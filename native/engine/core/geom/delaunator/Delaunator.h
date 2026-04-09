#pragma once
#include <vector>
#include <array>
#include <cstdint>
#include "../../math/Vec2.h"

/**
 * 传递进来一堆2D的离散的点，然后将这些点链接成可以渲染的三角形
 *  一个非常高效的三角剖分算法
 * 基于https://github.com/mapbox/delaunator 这个库实现的
 *
 */
namespace geometry2d
{

    class Delaunator
    {
    public:
        // 从 Vec2 点集创建（坐标单位保持原样）
        static Delaunator FromPoints(const std::vector<Vec2> &points);

        // 直接使用坐标数组创建 [x0,y0,x1,y1,...]
        explicit Delaunator(const std::vector<float> &coords);

        // 重新计算（可在修改 coords 后调用）
        void update(); // 更新三角剖分

        std::vector<float> coords;      // 输入坐标
        std::vector<uint32_t> triangles; // 三角形索引
        std::vector<int32_t> halfedges;  // 半边结构
        std::vector<uint32_t> hull;      // 凸包索引

    private:
        static constexpr float EPSILON = 1.0 / (1ULL << 52); // 与 JS 版一致的精度阈值
        static constexpr uint32_t EDGE_STACK_SIZE = 512;      // 固定栈大小

        // 内部缓存
        std::vector<uint32_t> _triangles; // 内部三角形数组
        std::vector<int32_t> _halfedges;  // 内部半边数组
        std::vector<uint32_t> _hullPrev;  // 凸包前驱
        std::vector<uint32_t> _hullNext;  // 凸包后继
        std::vector<uint32_t> _hullTri;   // 凸包关联三角形
        std::vector<int32_t> _hullHash;   // 凸包哈希表
        std::vector<uint32_t> _ids;       // 排序索引
        std::vector<float> _dists;       // 排序距离

        uint32_t _hashSize = 0;    // 哈希表大小
        uint32_t _hullStart = 0;   // 凸包起始点
        uint32_t trianglesLen = 0; // 有效三角形长度
        float _cx = 0.0;          // 外接圆中心 x
        float _cy = 0.0;          // 外接圆中心 y

        // 核心算法函数
        uint32_t _hashKey(float x, float y) const;                                                   // 计算哈希键
        uint32_t _legalize(uint32_t a, std::array<uint32_t, EDGE_STACK_SIZE> &stack);                  // Delaunay 边翻转
        void _link(uint32_t a, int32_t b);                                                             // 半边连接
        uint32_t _addTriangle(uint32_t i0, uint32_t i1, uint32_t i2, int32_t a, int32_t b, int32_t c); // 添加三角形

        // 数学工具
        static float orient2d(float ax, float ay, float bx, float by, float cx, float cy);                     // 方向测试
        static float pseudoAngle(float dx, float dy);                                                              // 伪角度
        static float dist(float ax, float ay, float bx, float by);                                               // 距离平方
        static bool inCircle(float ax, float ay, float bx, float by, float cx, float cy, float px, float py); // 圆内测试
        static float circumradius(float ax, float ay, float bx, float by, float cx, float cy);                 // 外接圆半径平方
        static Vec2 circumcenter(float ax, float ay, float bx, float by, float cx, float cy);                   // 外接圆圆心
        static void quicksort(std::vector<uint32_t> &ids, std::vector<float> &dists, int left, int right);           // 快排
        static void swap(std::vector<uint32_t> &arr, int i, int j);                                                   // 交换
    }; // 类结束
} // 命名空间结束
