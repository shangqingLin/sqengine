#pragma once
#include "../../common/constants-define.h"
#include "../../common/math/SqVec2.h"
namespace phxy
{
    /**
     * 用于为SqPolygonShape指定多边形的顶点
     */
    struct SqHull
    {
        SqVec2 points[SQ_MAX_POLYGON_VERTICES];
        int count{0};
    };

    SqHull SqComputeHull(const SqVec2 *points, int count);
    bool SqValidateHull(const SqHull *hull);
}