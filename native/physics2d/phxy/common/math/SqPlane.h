#pragma once
#include "SqVec2.h"

namespace phxy
{
    class SqPlane
    {

    public:
        SqVec2 normal;
        float offset{0.f};
        float separation(const SqVec2 &point);
    };

}