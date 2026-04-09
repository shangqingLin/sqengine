#pragma once

#include "Collision2DComponent.h"

namespace physics2d
{
    class BoxCollision2DComponent : public Collision2DComponent
    {
    private:
        float halfWidth = 0.f;
        float halfHeight = 0.f;
    public:
        BoxCollision2DComponent();
        virtual ~BoxCollision2DComponent();
        void create(float width, float height);
        virtual void setOffset(float x, float y);
    };

}
