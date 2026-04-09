#pragma once
#include "../SqStepContext.h"

namespace phxy
{

    struct SqContactConstraint
    {
        float normalImpulse;
        float tangentImpulse;
    };

    /**
     * Contact Solver的主要作用是处理碰撞响应流程(Collison Response)
     * 基于冲量实现碰撞响应。这里的目的就是计算发生碰撞的两个物体的冲量
     *
     * 实现公式：
     * https://en.wikipedia.org/wiki/Collision_response
     *
     */
    class SqBodySim;
    class SqManifoldPoint;
    class SqWorld;
    class SqContactSolver
    {
    protected:
        SqWorld *world;
        float calculateK(SqManifoldPoint &manifoldPoint, const SqVec2 &normal, SqBodySim *bodySimA, SqBodySim *bodySimB, float &relVelocity);
    public:
        SqContactSolver(SqWorld *);
        void warmStart(SqStepContext &context);
        virtual void prepare(SqStepContext &context);
        virtual void solve(SqStepContext &context, bool useBias) = 0;
    };

}