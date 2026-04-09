#pragma once
#include "../Array.h"
#include "../../math/Vec2.h"

namespace sqstd
{

    /**
     * 逆时针排序点
     */
    void SortPoints2DCCW(sqstd::Array<Vec2>& points);
}