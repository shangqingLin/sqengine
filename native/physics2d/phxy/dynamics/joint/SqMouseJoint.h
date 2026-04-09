#pragma once
#include "SqJoint.h"
#include "../../common/math/SqMat22.h"
namespace phxy
{
    class SqMouseJoint : public SqJoint
    {
    private:
        float hertz = 0.f;

        /**
         * 小于0表示禁用最大力度限制
         */
        float maxForce = 0.f;
        
        float dampingRatio = 0.f;

        SqTransform frameA;
        SqTransform frameB;
        SqVec2 deltaCenter;

        SqSoftConstraint linearSoftness;
        SqSoftConstraint angularSoftness;
        SqMat22 linearMass;
        float angularMass = 0.f;

        SqVec2 linearImpulse;
        float angularImpulse = 0.f;

    protected:
        virtual void solvePrepare(const SqStepContext &context);

    public:
        friend class SqWorld;
        SqMouseJoint();
        void SetSpringHertz(float hertz);
        float GetSpringHertz();
        void SetSpringDampingRatio(float dampingRatio);
        float GetSpringDampingRatio();
        void SetMaxForce(float maxForce);
        float GetMaxForce();

        virtual void warmStart(const SqStepContext &context);
        virtual void solve(const SqStepContext &context, bool useBias);
    };
}