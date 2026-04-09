#pragma once
#include "Joint2DComponent.h"

namespace physics2d
{
    class MouseJoint2DComponent : public Joint2DComponent
    {
    protected:
        virtual void onSwitchConnectNode();
        virtual  phxy::SqBaseJointDef *onCreateJointDef();

    public:
        MouseJoint2DComponent();
        void setTarget(Vec2 &vec);
        void setHertz(float v);
        void setDampingRatio(float v);
        void setMaxForce(float v);
    };
}
