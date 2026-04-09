#pragma once
#include "SqManifold.h"
#include "../../geometry/shape/SqCapsuleShape.h"
#include "../../geometry/shape/SqCircleShape.h"
#include "../../common/math/SqTransform.h"

namespace phxy
{

    SqManifold sqCollideCapsuleAndCircle( const SqCapsuleShape* capsuleA, SqTransform& xfA, const SqCircleShape* circleB, SqTransform& xfB );
}