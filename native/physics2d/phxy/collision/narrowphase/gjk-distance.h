#pragma once
#include "collision.h"

namespace phxy
{
    struct SqSimplexCache
    {
        /// The number of stored simplex points
        uint16_t count{0};

        /// The cached simplex indices on shape A
        uint8_t indexA[3];

        /// The cached simplex indices on shape B
        uint8_t indexB[3];
    };

    /**
     * 单纯形三角形的顶点数据结构
     */
    struct SqSimplexVertex
    {
        /**
         * wA和wB表示当前的顶点是由多边形上哪个顶点组成，在gjk算法称为support point
         * wA表示是proxyA多边形上的点
         * wB表示是proxyB多边形上的点
         */
        SqVec2 wA;
        SqVec2 wB;
        SqVec2 w;   /// wB - wA 即这两个support点组成的单纯形中的一个点
        float a{0.f};    /// barycentric coordinate for closest point
        int indexA = -1; /// wA 点在多边形proxyA数组中的index
        int indexB = -1; /// wB 点在多边形proxyB数组中的index
    };

    
    /**
     * 定义GJK算法中的单纯形
     */
    struct SqSimplex
    {
        //单纯形中的三个顶点
        SqSimplexVertex v1, v2, v3; ///< vertices
        int count{0};                  ///< number of valid vertices
    };

    int sqFindSupport( const phxy::SqShapeProxy* proxy,const SqVec2& direction);
    SqDistanceOutput sqShapeDistance(const SqDistanceInput *input, SqSimplexCache *cache, SqSimplex *simplexes, int simplexCapacity);
}