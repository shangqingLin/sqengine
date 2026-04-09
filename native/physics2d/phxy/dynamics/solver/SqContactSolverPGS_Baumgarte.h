#pragma once
#include "SqContactSolver.h"

namespace phxy
{
    class SqContactSolverPGS_Baumgarte : public SqContactSolver
    {
    public:
        SqContactSolverPGS_Baumgarte(SqWorld*);
        virtual void solve(SqStepContext &context, bool useBias);
    };

}