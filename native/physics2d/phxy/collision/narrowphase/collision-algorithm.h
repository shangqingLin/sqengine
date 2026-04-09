#pragma once
#include "SqManifold.h"
#include "../../geometry/shape/SqPolygonShape.h"

namespace phxy
{
    SqManifold sqClipPolygons(const SqPolygonShape *polyA, const SqPolygonShape *polyB, int edgeA, int edgeB, bool flip);
    float sqFindMaxSeparation(int *edgeIndex, const SqPolygonShape *poly1, const SqPolygonShape *poly2);
    SqManifold sqClipSegments(SqVec2 a1, SqVec2 a2, SqVec2 b1, SqVec2 b2, SqVec2 normal, float ra, float rb, uint16_t id1, uint16_t id2);
}