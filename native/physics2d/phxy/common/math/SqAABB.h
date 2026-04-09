#pragma once
#include "SqVec2.h"

namespace phxy
{
    /**
     * 定义轴对称包围盒
     */
    class SqAABB
    {
    public:

        /**
         * AABB盒子的左下角
         */
        SqVec2 lowerBound;

        /**
         * AABB盒子的右上角
         */
        SqVec2 upperBound;

        /**
         * 将两AABB合并为一个AABB
         */
        static SqAABB Union(const SqAABB& a,const SqAABB& b );

        /**
         * 计算包围盒的中心点的坐标
         */
        static SqVec2 Center(const SqAABB& a );
        static SqVec2 Extents(const SqAABB &a);

        /**
         * 计算包围盒的面积
         */
        static float Perimeter(const SqAABB& a );

        static bool isValid(const SqAABB& a );

        /**
         * 根据b扩展a
         * 如果b的某一维度比a的大，则扩展a这个维度，如果小则不变a
         * @return 返回a是否被更改了
         */
        static bool enlargeAABB(SqAABB *a, const SqAABB& b);

        /**
         * 判断b是否完全包含在a内，是完全包含哦
         */
        static bool Contains(const SqAABB& a, const SqAABB& b );

        /**
         * 判断a和b是否有重叠，只要发生重叠就返回true
         */
        static bool Overlaps(const SqAABB& a,const SqAABB& b );

        static SqAABB MakeAABB(const SqVec2 *points, int count, float radius);
    };
}