#include "SqJointSolver.h"
#include "../SqWorld.h"

using namespace phxy;

SqJointSolver::SqJointSolver(SqWorld *world) : world(world) {}

void SqJointSolver::warmStart(SqStepContext &context)
{
    SqSolverSet *awakeSet = world->getSloverSet(sq_awakeSet);
    int count = awakeSet->jointSims.getCount();
    for (int i = 0; i < count; ++i)
    {
        SqJointSim *sim = awakeSet->jointSims.get(i);
        SqJoint *joint = context.world->getJoint(sim->jointId);
        if (joint->isEnableSim())
        {
            joint->warmStart(context);
        }
    }
}

void SqJointSolver::prepare(SqStepContext &context)
{
    SqSolverSet *awakeSet = world->getSloverSet(sq_awakeSet);
    int count = awakeSet->jointSims.getCount();
    for (int i = 0; i < count; ++i)
    {
        SqJointSim *sim = awakeSet->jointSims.get(i);
        SqJoint *joint = context.world->getJoint(sim->jointId);
        if (joint->isEnableSim())
        {
            joint->prepare(context);
        }
    }
}

void SqJointSolver::solve(SqStepContext &context, bool bias)
{
    SqSolverSet *awakeSet = world->getSloverSet(sq_awakeSet);
    int count = awakeSet->jointSims.getCount();
    for (int i = 0; i < count; ++i)
    {
        SqJointSim *sim = awakeSet->jointSims.get(i);
        SqJoint *joint = context.world->getJoint(sim->jointId);
        if (joint->isEnableSim())
        {
            joint->solve(context, bias);
        }
    }
}