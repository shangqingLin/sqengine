#pragma once

#include "Collision2DComponent.h"

namespace physics2d{
    class CircleCollision2DComponent : public Collision2DComponent
    {
     public:
        CircleCollision2DComponent();
        void setRadius(float);
        float getRadius();
        virtual void setOffset(float x,float y);
    };
}
