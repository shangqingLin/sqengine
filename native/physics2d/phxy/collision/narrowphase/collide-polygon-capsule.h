#pragma once
#include "SqManifold.h"
#include "../../geometry/shape/SqPolygonShape.h"
#include "../../geometry/shape/SqCapsuleShape.h"
#include "../../common/math/SqTransform.h"
namespace phxy
{
    SqManifold sqCollidePolygonAndCapsule( const SqPolygonShape* polygonA, SqTransform& xfA, const SqCapsuleShape* capsuleB, SqTransform& xfB );
}