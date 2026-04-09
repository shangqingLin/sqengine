#pragma once
#include "SqManifold.h"
#include "../../common/math/SqTransform.h"
#include "../../geometry/shape/SqChainEdgeShape.h"
#include "../../geometry/shape/SqCircleShape.h"
namespace phxy
{
    SqManifold sqCollideChainSegmentAndCircle( const SqChainEdgeShape* segmentA, SqTransform& xfA, const SqCircleShape* circleB,
										   SqTransform& xfB );
}