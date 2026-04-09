#pragma once
#include "SqManifold.h"
#include "../../common/math/SqTransform.h"
#include "../../geometry/shape/SqPolygonShape.h"
#include "../../geometry/shape/SqEdgeShape.h"
namespace phxy
{
    SqManifold sqCollideSegmentAndPolygon( const SqEdgeShape* segmentA, SqTransform& xfA, const SqPolygonShape* polygonB, SqTransform& xfB );
}