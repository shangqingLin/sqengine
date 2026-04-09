#pragma once
#include "../../engine/core/math/Vec2.h"
#include <optional>
#include "../phxy/sq-phxy.h"


namespace physics2d
{
    enum class ShapeType
    {
        box,
        circle,
        mesh
    };

    struct BodyDef
    {
        bool allowSleep;
        float angularDamping;
        float linearDamping;
        bool bullet;
        phxy::SqBodyType type;
        std::optional<Vec2> position;
        std::optional<Vec2> rotation;
    };
}