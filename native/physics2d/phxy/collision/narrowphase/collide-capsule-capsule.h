#pragma once
#include "../../common/math/SqTransform.h"
#include "../../geometry/shape/SqCapsuleShape.h"
#include "SqManifold.h"

namespace phxy
{
	SqManifold sqCollideCapsules( const SqCapsuleShape* capsuleA, SqTransform& xfA, const SqCapsuleShape* capsuleB, SqTransform& xfB );
}