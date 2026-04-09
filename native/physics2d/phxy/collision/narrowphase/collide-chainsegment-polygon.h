#pragma once
#include "SqManifold.h"
#include "../../geometry/shape/SqChainEdgeShape.h"
#include "../../common/math/SqTransform.h"
#include "../../geometry/shape/SqPolygonShape.h"
#include "gjk-distance.h"

namespace phxy
{
    SqManifold sqCollideChainSegmentAndPolygon(const SqChainEdgeShape *segmentA, SqTransform &xfA, const SqPolygonShape *polygonB,
                                               SqTransform &xfB, SqSimplexCache *cache);
}