#pragma once
#include "Collision2DComponent.h"

namespace physics2d
{
    class PolygonCollision2DComponent : public Collision2DComponent
    {
    public:
        PolygonCollision2DComponent();
        void setPoints(float *points, int numPoint);
    };
}
