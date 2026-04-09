#pragma once
#include "SqManifold.h"
#include "../../common/math/SqTransform.h"
#include "../../geometry/shape/SqPolygonShape.h"
#include "../../geometry/shape/SqCircleShape.h"
namespace phxy
{
    SqManifold sqCollidePolygonAndCircle( const SqPolygonShape* polygonA,SqTransform& xfA, const SqCircleShape* circleB,SqTransform& xfB );
}

