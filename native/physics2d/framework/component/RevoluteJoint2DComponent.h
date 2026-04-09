#pragma once
#include "Joint2DComponent.h"

namespace physics2d
{
    class RevoluteJoint2DComponent : public Joint2DComponent
    {
    protected:
        virtual void onSwitchConnectNode();
        virtual phxy::SqBaseJointDef *onCreateJointDef();

    public:
        void setTargetAngle(float angle);
        void setEnableSpring(bool enable);
        void setHertz(float v);
        void setDampingRatio(float v);
        void setEnableLimit(bool enable);
        void setLowerAngle(float angle);
        void setUpperAngle(float angle);
        void setEnableMotor(bool enable);
        void setMaxMotorTorque(float v);
        void setMotorSpeed(float v);
       
    };
}
