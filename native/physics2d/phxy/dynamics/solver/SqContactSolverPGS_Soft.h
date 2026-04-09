#pragma once

#include "SqContactSolver.h"

namespace phxy
{
    class SqContactSolverPGS_Soft : public SqContactSolver
    {
    private:
        void solveNonPenetrationConstraint(SqStepContext &context, SqContactSim *contactSim, SqManifoldPoint &manifold, bool useBias);
        void solveFrictionConstraint(SqStepContext &context, SqContactSim *contactSim, SqManifoldPoint &manifold, bool useBias);
        void solveRollingResistance(SqStepContext &context, SqContactSim *contactSim, SqManifoldPoint &manifold, bool useBias);
        void solveSIMD(SqStepContext &context);
    public:
        SqContactSolverPGS_Soft(SqWorld *);
        virtual void solve(SqStepContext &context, bool useBias);
    };
}