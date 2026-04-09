#pragma once

#include "SqManifold.h"
#include "../../common/math/SqTransform.h"
#include "../../geometry/shape/SqPolygonShape.h"

namespace phxy
{
    SqManifold sqCollidePolygons(const SqPolygonShape *polygonA, SqTransform& xfA, const SqPolygonShape *polygonB, SqTransform& xfB);
}