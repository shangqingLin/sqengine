#pragma once
#include "../../common/constants-define.h"

namespace phxy
{
    class SqShape;
    class SqWorld;
    class SqBody;
    struct SqShapeFilter
    {
        uint64_t categoryBits{SQ_DEFAULT_CATEGORY_BITS};
        uint64_t maskBits{SQ_DEFAULT_MASK_BITS};
    };



    bool SqShouldShapesCollide(const SqShapeFilter& filterA,const SqShapeFilter& filterB);
    bool SqShouldBodiesCollide(SqWorld*,SqBody *bodyA, SqBody *bodyB);
    bool SqBroadShapeFilterCollide(SqWorld*, SqShape*,SqShape*);
}