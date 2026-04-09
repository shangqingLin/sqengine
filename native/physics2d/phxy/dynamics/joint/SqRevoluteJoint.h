#pragma once
#include "SqJoint.h"
#include "../SqSoftConstraint.h"

namespace phxy
{
    class SqRevoluteJoint : public SqJoint
    {
    private:
        float targetAngle = 0.f;
        bool enableSpring = false;
        float springImpulse = 0.f;
        float hertz = 0.f;
        float dampingRatio = 0.f;
        bool enableLimit = false;
        float lowerAngle = 0.0f;
        float upperAngle = 0.f;
        float lowerImpulse = 0.f;
        float upperImpulse = 0.f;
        bool enableMotor = false;
        float motorImpulse = 0.f;
        float maxMotorTorque = 0.f;
        float motorSpeed = 0.f;

        float axialMass = 0.f;
        SqVec2 deltaCenter;
        SqTransform frameA;
        SqTransform frameB;
        SqVec2 linearImpulse;
        SqSoftConstraint springSoftness;

    protected:
        virtual void solvePrepare(const SqStepContext &context);

    public:
        friend class SqWorld;
        SqRevoluteJoint();
        void setEnableSpring(bool enableSpring);
        bool isSpringEnabled();
        void setSpringHertz(float hertz);
        float getSpringHertz();
        void setSpringDampingRatio(float dampingRatio);
        float getSpringDampingRatio();
        void setTargetAngle(float angle);
        float getTargetAngle();
        float getAngle();
        void setEnableLimit(bool enableLimit);
        bool isLimitEnabled();
        float getLowerLimit();
        float getUpperLimit();
        void setLimits(float lower, float upper);
        void setEnableMotor(bool enableMotor);
        bool isMotorEnabled();
        void setMotorSpeed(float motorSpeed);
        float getMotorSpeed();
        float getMotorTorque();
        void setMaxMotorTorque(float torque);
        float getMaxMotorTorque();
        SqVec2 getRevoluteJointForce();
        float getRevoluteJointTorque();
        virtual void setEnableSim(bool b);
        virtual void warmStart(const SqStepContext &context);
        virtual void solve(const SqStepContext &context, bool useBias);

        void test(SqBodySim *stateA, SqBodySim *stateB, float wA, float wB, const SqStepContext &context, float C);
    };
}