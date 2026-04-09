#pragma once
#include "SqJoint.h"
#include "../../common/math/SqMat22.h"

namespace phxy
{
    class SqMotorJoint : public SqJoint
    {
    private:
        float maxForce = 0.f;
        float maxTorque = 0.f;
        float correctionFactor = 0.f;
        SqTransform frameA;
        SqTransform frameB;
        SqVec2 deltaCenter;

        SqMat22 linearMass;
        float angularMass = 0.f;
        SqVec2 linearImpulse;
        float angularImpulse = 0.0f;

    protected:
        virtual void solvePrepare(const SqStepContext &context);

    public:
        friend class SqWorld;
        SqMotorJoint();
        void SetMaxForce(float maxForce);
        float GetMaxForce();
        void SetMaxTorque(float maxTorque);
        float GetMaxTorque();
        void SetCorrectionFactor(float correctionFactor);
        float GetCorrectionFactor();

        virtual void warmStart(const SqStepContext &context);
        virtual void solve(const SqStepContext &context, bool useBias);
    };

}