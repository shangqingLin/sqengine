#pragma once

#include "Joint2DComponent.h"
namespace physics2d
{
    class WheelJoint2DComponent : public Joint2DComponent
    {
    protected:
        virtual void onSwitchConnectNode();
        virtual phxy::SqBaseJointDef *onCreateJointDef();

    public:
        void setEnableSpring(float enable);
        void setHertz(float v);
        void setDampingRatio(float v);
        void setEnableLimit(bool enable);
        void setLowerTranslation(float angle);
        void setUpperTranslation(float angle);
        void setEnableMotor(bool enable);
        void setMaxMotorTorque(float v);
        void setMotorSpeed(float v);
        void setLocalAxisA(Vec2 &v);
    };
}