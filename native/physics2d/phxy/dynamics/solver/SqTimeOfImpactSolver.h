#pragma once

#include "../SqStepContext.h"

/**
 * 实现连续碰撞检测TOI，防止过快的物体穿透
 */

namespace phxy
{
    class SqWorld;
    class SqTimeOfImpactSolver
    {
    private:
        SqWorld *world;

    public:
        SqTimeOfImpactSolver(SqWorld *world);
        bool checkBodyNeedContinuous(SqBodySim*);
        void solveContinuous(SqBodySim*);
        void solve(SqStepContext &);
    };
}