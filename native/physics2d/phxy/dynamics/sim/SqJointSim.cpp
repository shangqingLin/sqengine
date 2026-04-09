#include "SqJointSim.h"

using namespace phxy;

SqJointSim::SqJointSim()
{
    reset();
}

void SqJointSim::reset()
{
    jointId = SQ_NULL_INDEX;
    bodyAIndex = SQ_NULL_INDEX;
    bodyBIndex = SQ_NULL_INDEX;
    localFrameA.identity();
    localFrameB.identity();
    enableSoftness = true;
    constraintHertz = 60.f;
    constraintDampingRatio = 0.f;
    forceThreshold = 0.f;
    torqueThreshold = 0.f;
    constraintSoftness.biasRate = 0.f;
    constraintSoftness.impulseScale = 0.f;
    constraintSoftness.massScale = 1.0f;
}