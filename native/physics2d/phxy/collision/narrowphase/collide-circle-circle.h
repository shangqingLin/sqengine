#pragma once
#include "SqManifold.h"
#include "../../common/math/SqTransform.h"
#include "../../geometry/shape/SqCircleShape.h"

namespace phxy
{
    SqManifold sqCollideCircles( const SqCircleShape* circleA,SqTransform& xfA, const SqCircleShape* circleB,SqTransform& xfB );
}