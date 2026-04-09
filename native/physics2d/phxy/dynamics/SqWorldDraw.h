#pragma once
#include "../common/SqTypeDefine.h"

namespace phxy
{
    class SqWorld;
    struct SqDebugDraw;
    class SqTransform;
    class SqShape;
    class SqJoint;
    class SqWorldDraw
    {
    private:
        void drawWithBounds(SqWorld *world, SqDebugDraw *draw);
        void drawJoint(SqDebugDraw *draw, SqWorld *world, SqJoint *joint);
    public:
        void drawShape(SqDebugDraw *draw, SqShape *shape, SqTransform xf, SqHexColor color);
        void debugDraw(SqWorld *world, SqDebugDraw *draw);
    };
}