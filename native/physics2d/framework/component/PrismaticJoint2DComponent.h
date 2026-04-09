#pragma once
#include "Joint2DComponent.h"

namespace physics2d
{
    class PrismaticJoint2DComponent : public Joint2DComponent
    {
    protected:
        virtual void onSwitchConnectNode();
        virtual phxy::SqBaseJointDef *onCreateJointDef();

    public:
        void setEnableSpring(bool enable);
        void setHertz(float v);
        void setDampingRatio(float v);
        void setEnableLimit(bool enable);
        void setLowerTranslation(float angle);
        void setUpperTranslation(float angle);
        void setEnableMotor(bool enable);
        void setMaxMotorForce(float v);
        void setMotorSpeed(float v);
        void SetEnableSyncAngle(bool enableSyncAngle);
    };

}