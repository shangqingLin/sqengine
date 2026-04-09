#pragma once

#include "SqManifold.h"
#include "../../common/math/SqTransform.h"
#include "../../geometry/shape/SqCapsuleShape.h"
#include "../../geometry/shape/SqEdgeShape.h"

namespace phxy
{
    SqManifold sqCollideSegmentAndCapsule( const SqEdgeShape* segmentA, SqTransform xfA, const SqCapsuleShape* capsuleB, SqTransform xfB );
}