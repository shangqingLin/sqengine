#pragma once
#include "Collision2DComponent.h"

namespace physics2d
{
    class CapsuleCollision2DComponent : public Collision2DComponent
    {
    private:
        Vec2 center1;
        Vec2 center2;

    public:
        CapsuleCollision2DComponent();
        virtual ~CapsuleCollision2DComponent();
        virtual void setOffset(float x, float y);
        void create(float p1x, float p1y, float p2x, float p2y, float radius);
    };
}
