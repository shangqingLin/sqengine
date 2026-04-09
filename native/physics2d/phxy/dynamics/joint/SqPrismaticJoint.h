#pragma once
#include "SqJoint.h"

namespace phxy
{
    class SqPrismaticJoint : public SqJoint
    {
    private:
        bool enableSyncAngle = true;
        bool enableSpring = false;
        float springImpulse = 0.f;
        float hertz = 0.f;
        float dampingRatio = 0.f;
        float targetTranslation = 0.f;

        float enableLimit = false;
        float lowerImpulse = 0.0f;
        float upperImpulse = 0.0f;

        float lowerTranslation = 0.f;
        float upperTranslation = 0.f;

        bool enableMotor = false;
        float motorImpulse = 0.0f;
        float motorSpeed = 0.f;

        float maxMotorForce = 0.f;

        SqTransform frameA;
        SqTransform frameB;
        SqVec2 deltaCenter;

        float axialMass = 0.f;
        SqSoftConstraint springSoftness;

        SqVec2 impulse;

    protected:
        virtual void solvePrepare(const SqStepContext &context);

    public:
        friend class SqWorld;

        SqPrismaticJoint();
        void EnableSpring(bool enableSpring);
        bool IsSpringEnabled();
        void SetSpringHertz(float hertz);
        float GetSpringHertz();
        void SetSpringDampingRatio(float dampingRatio);
        float GetSpringDampingRatio();
        void SetTargetTranslation(float translation);
        float GetTargetTranslation();
        void EnableLimit(bool enableLimit);
        bool IsLimitEnabled();
        float GetLowerLimit();
        float GetUpperLimit();
        void SetLimits(float lower, float upper);
        void EnableMotor(bool enableMotor);
        bool IsMotorEnabled();
        void SetMotorSpeed(float motorSpeed);
        float GetMotorSpeed();
        float GetMotorForce();
        void SetMaxMotorForce(float force);
        float GetMaxMotorForce();
        float GetTranslation();
        float GetSpeed();
        void SetEnableSyncAngle(bool enableSyncAngle);
        virtual void warmStart(const SqStepContext &context);
        virtual void solve(const SqStepContext &context, bool useBias);
    };

}