#pragma once
#include "SqJoint.h"
namespace phxy
{
    class SqDistanceJoint : public SqJoint
    {
    private:
        float length = 0.f;
        float impulse = 0.0f;
        float lowerImpulse = 0.0f;
        float upperImpulse = 0.0f;
        bool enableLimit = false;
        float minLength = 0.f;
        float maxLength = 0.f;

        bool enableSpring = false;
        float hertz = 0.f;
        float dampingRatio = 0.f;

        bool enableMotor = false;
        float motorSpeed = 0.f;
        float maxMotorForce = 0.f;
        float motorImpulse = 0.f;

        SqVec2 anchorA;
        SqVec2 anchorB;
        SqVec2 deltaCenter;

        float axialMass = 0.f;
        SqSoftConstraint distanceSoftness;

    protected:
        virtual void solvePrepare(const SqStepContext &context);

    public:
        friend class SqWorld;

        SqDistanceJoint();

        void SetLength(float length);
        float GetLength();
        void EnableLimit(bool enableLimit);
        bool IsLimitEnabled();
        void SetLengthRange(float minLength, float maxLength);
        float GetMinLength();
        float GetMaxLength();
        float GetCurrentLength();
        void EnableSpring(bool enableSpring);
        bool IsSpringEnabled();
        void SetSpringHertz(float hertz);
        void SetSpringDampingRatio(float dampingRatio);
        float GetSpringHertz();
        float GetSpringDampingRatio();
        void EnableMotor(bool enableMotor);
        bool IsMotorEnabled();
        void SetMotorSpeed(float motorSpeed);
        float GetMotorSpeed();
        float GetMotorForce();
        float GetMaxMotorForce();
        void SetMaxMotorForce(float force);
        virtual void setEnableSim(bool b);
        virtual void warmStart(const SqStepContext &context);
        virtual void solve(const SqStepContext &context, bool useBias);
    };

}