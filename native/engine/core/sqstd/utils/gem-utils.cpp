#include "gem-utils.h"

void sqstd::SortPoints2DCCW(sqstd::Array<Vec2> &points)
{
    /**
     * 核心思想
     *   1、找质心 C。因为点的排序肯定需要查照点的。
     *      比如说任意的三个点，如果没有参考点的话他可以是逆时针，也可以是顺时针，程序不知道。
     *      这固定参考点为质心就行
     *
     *   2、将每个点向量 v = P - C
     *   3、先按点在 左半平面 或 右半平面 分类。再按 cross(vA, vB) 比较谁的极角更小
     */

     
    int count = points.getCount();
    Vec2 c(0.f, 0.f);
    // 计算中心点
    for (int i = 0; i < points.getCount(); ++i)
    {
        Vec2 *point = points.get(i);
        c.x += point->x;
        c.y += point->y;
    }

    c.x = c.x / (float)count;
    c.y = c.y / (float)count;

    points.sort([&c](const Vec2 &A, const Vec2 &B)
                {
        Vec2 a{A.x - c.x, A.y - c.y};
        Vec2 b{B.x - c.x, B.y - c.y};

        bool upperA = (a.y > 0) || (a.y == 0 && a.x >= 0);
        bool upperB = (b.y > 0) || (b.y == 0 && b.x >= 0);
        if (upperA != upperB) return upperA;    // upper first

        float cr = Vec2::cross(a, b);
        if (cr != 0) return cr > 0;             // a before b if cross>0

        // same angle -> nearer one first
        float da = a.x * a.x + a.y * a.y;
        float db = b.x * b.x + b.y * b.y;
        return da < db; });
}