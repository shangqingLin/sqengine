#pragma once
#include "SqManifold.h"
#include "../../geometry/shape/SqChainEdgeShape.h"
#include "../../common/math/SqTransform.h"
#include "../../geometry/shape/SqCapsuleShape.h"
#include  "gjk-distance.h"

namespace phxy
{
    SqManifold sqCollideChainSegmentAndCapsule( const SqChainEdgeShape* segmentA, SqTransform& xfA, const SqCapsuleShape* capsuleB,
											SqTransform& xfB, SqSimplexCache* cache );

}