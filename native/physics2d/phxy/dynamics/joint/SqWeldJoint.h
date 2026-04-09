#pragma once

#include "SqJoint.h"
#include "../SqSoftConstraint.h"

namespace phxy
{

    class SqWeldJoint : public SqJoint
    {
    private:
        float linearHertz = 0.f;
        float linearDampingRatio = 0.f;
        float angularHertz = 0.f;
        float angularDampingRatio = 0.f;

        SqTransform frameA;
        SqTransform frameB;

        SqVec2 deltaCenter;

        float axialMass = 0.f;
        SqVec2 linearImpulse;
        float angularImpulse = 0.0f;

        SqSoftConstraint linearSoftness;
        SqSoftConstraint angularSoftness;

    protected:
        virtual void solvePrepare(const SqStepContext &context);

    public:
        friend class SqWorld;
        SqWeldJoint();
        void SetLinearHertz(float hertz);
        float GetLinearHertz();
        void SetLinearDampingRatio(float dampingRatio);
        float GetLinearDampingRatio();
        void SetAngularHertz(float hertz);
        float GetAngularHertz();
        void SetAngularDampingRatio(float dampingRatio);
        float GetAngularDampingRatio();

        virtual void warmStart(const SqStepContext &context);
        virtual void solve(const SqStepContext &context, bool useBias);
    };
}