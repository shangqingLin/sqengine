#pragma once
#include "../SqStepContext.h"
namespace phxy
{
    class SqWorld;
    class SqJointSolver
    {
    private:
        SqWorld *world;

    public:
        SqJointSolver(SqWorld *);
        void warmStart(SqStepContext &context);
        void prepare(SqStepContext &context);
        void solve(SqStepContext &context,bool);
    };
}