#pragma once
#include "Joint2DComponent.h"

namespace physics2d
{
    class WeldJoint2DComponent : public Joint2DComponent
    {
    protected:
        virtual void onSwitchConnectNode();
        virtual  phxy::SqBaseJointDef *onCreateJointDef();

    public:
        void setLinearHertz(float v);
        void setAngularHertz(float v);
        void setLinearDampingRatio(float v);
        void setAngularDampingRatio(float v);
    };

}
