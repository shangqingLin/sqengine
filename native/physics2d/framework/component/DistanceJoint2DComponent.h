#pragma once
#include "Joint2DComponent.h"

namespace physics2d
{
    class DistanceJoint2DComponent : public Joint2DComponent
    {
    protected:
        virtual void onSwitchConnectNode();
        virtual  phxy::SqBaseJointDef *onCreateJointDef();

    public:
        void setLength(float v);
        void setEnableSpring(bool enable);
        void setHertz(float v);
        void setDampingRatio(float v);
        void setEnableLimit(bool enable);
        void setMaxLength(float v);
        void setMaxMotorForce(float v);
        void setEnableMotor(bool enable);
        void setMinLength(float v);
        void setMotorSpeed(float v);
    };
}
