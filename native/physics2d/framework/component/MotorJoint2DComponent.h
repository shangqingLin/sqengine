#pragma once
#include "Joint2DComponent.h"

namespace physics2d
{
    class MotorJoint2DComponent : public Joint2DComponent
    {
    protected:
        virtual void onSwitchConnectNode();
        virtual  phxy::SqBaseJointDef *onCreateJointDef();

    public:
        void setMaxForce(float v);
        void setMaxTorque(float v);
        void setCorrectionFactor(float v);
    };
}
