#pragma once
#include "SqJoint.h"

namespace phxy
{
    class SqWheelJoint : public SqJoint
    {
    private:
        bool enableSpring = false;
        float hertz = 1.f;
        float dampingRatio = 0.7f;

        bool enableLimit = false;
        float lowerImpulse = 0.0f;
        float upperImpulse = 0.0f;

        float lowerTranslation = 0.f;
        float upperTranslation = 0.f;

        bool enableMotor = false;
        float motorImpulse = 0.f;

        float motorSpeed = 0.f;

        float maxMotorTorque = 0.f;

        SqTransform frameA;
        SqTransform frameB;

        SqVec2 deltaCenter;

        float perpMass = 0.f;
        float axialMass = 0.f;
        float motorMass = 0.f;

        SqSoftConstraint springSoftness;

        float perpImpulse = 0.0f;
        float springImpulse = 0.0f;

    protected:
        virtual void solvePrepare(const SqStepContext &context);

    public:
        friend class SqWorld;
        SqWheelJoint();
        void EnableSpring(bool enableSpring);
        bool IsSpringEnabled();
        void SetSpringHertz(float hertz);
        float GetSpringHertz();
        void SetSpringDampingRatio(float dampingRatio);
        float GetSpringDampingRatio();
        void EnableLimit(bool enableLimit);
        bool IsLimitEnabled();
        float GetLowerLimit();
        float GetUpperLimit();
        void SetLimits(float lower, float upper);
        void EnableMotor(bool enableMotor);
        bool IsMotorEnabled();
        void SetMotorSpeed(float motorSpeed);
        float GetMotorSpeed();
        float GetMotorTorque();
        void SetMaxMotorTorque(float torque);
        float GetMaxMotorTorque();
        SqVec2 GetWheelJointForce();
        virtual void warmStart(const SqStepContext &context);
        virtual void solve(const SqStepContext &context, bool useBias);
    };
}