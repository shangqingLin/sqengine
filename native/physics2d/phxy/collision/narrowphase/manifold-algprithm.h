#pragma once
#include "SqManifold.h"
#include "collision.h"

namespace phxy
{
    SqSegmentDistanceResult sqSegmentDistance(SqVec2 p1, SqVec2 q1, SqVec2 p2, SqVec2 q2);
}