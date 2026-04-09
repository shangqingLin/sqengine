#pragma once
#include "SqManifold.h"
#include "../../common/math/SqTransform.h"
#include "../../geometry/shape/SqCircleShape.h"
#include "../../geometry/shape/SqEdgeShape.h"
namespace phxy
{
    SqManifold sqCollideSegmentAndCircle( const SqEdgeShape* segmentA, SqTransform& xfA, const SqCircleShape* circleB, SqTransform& xfB );
}