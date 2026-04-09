#pragma once
#include "../SqStepContext.h"
#include "SqContactSolver.h"
#include "SqBodySolver.h"
#include "SqTimeOfImpactSolver.h"
#include "SqJointSolver.h"
#include "SqContactSolverPGS_Baumgarte.h"
#include "SqContactSolverPGS_Soft.h"

namespace phxy
{
    class SqWorld;
    class SqSolver
    {
    private:
        SqWorld *world;
        SqBodySolver bodySovler;
        SqContactSolverPGS_Soft contanctSlover;
        // SqContactSolverPGS_Baumgarte contanctSlover;
        SqTimeOfImpactSolver TOISlover;
        SqJointSolver jointSolver;

        void islandSplitProcess(SqStepContext &context);
        void islandSleepProcess(SqStepContext &context);

    public:
        SqSolver(SqWorld *);
        void solve(SqStepContext &context);
    };
}
