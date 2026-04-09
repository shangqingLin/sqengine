#pragma once
#include "../SqStepContext.h"

namespace phxy
{

    class SqWorld;
    class SqBodySolver
    {
    private:
        SqWorld* world;
        void bodyVelocities(SqBodySim *bodySim, SqStepContext &);
        void bodyPosition(SqBodySim *bodySim, SqStepContext &);
        void finalizeTransform(SqBodySim *bodySim, SqStepContext &);
    public:
        SqBodySolver(SqWorld*);
        void solveBodyVelocities(SqStepContext &);
        void sloveBodyPosition(SqStepContext &);
        void sloveTransform(SqStepContext &);
        void syncProxy(SqStepContext &);
        void showBodyInfo(SqStepContext&);
    };

}
