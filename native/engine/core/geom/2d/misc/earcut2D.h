#pragma once
#include "../../../sqstd/sqstd.h"
#include "../../../math/Vec2.h"

/**
 * earcut算法实现对2D平面多边形进行三角剖分的算法
 */

void earcut(sqstd::Array<Vec2 *> &points, sqstd::Array<unsigned short> &triangles, int baseVertex = 0, bool passZOrder = false);
void earcut(sqstd::Array<Vec2> &points, sqstd::Array<unsigned short> &triangles, int baseVertex = 0, bool passZOrder = false);
