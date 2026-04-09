#include "SqJoint.h"
#include "../SqWorld.h"
#include "../../common/SqCore.h"
#include "../../common/math/SqMath.h"

using namespace phxy;

void SqJoint::reset()
{

    jointIndex = SQ_NULL_INDEX;
    simIndex = SQ_NULL_INDEX;
    setIndex = SQ_NULL_INDEX;
    islandPrev = SQ_NULL_INDEX;
    islandNext = SQ_NULL_INDEX;
    islandId = SQ_NULL_INDEX;
    isMarkedForIslandSplit = false;
    deleteCallback = nullptr;
    deleteCallbackContext = nullptr;
}

SqJointSim *SqJoint::getJointSim() const
{
    SqSolverSet *set = world->getSloverSet(setIndex);
    SQ_ASSERT(set);
    return set->jointSims.get(simIndex);
}

bool SqJoint::isEnable()
{
    // 大于sq_disabledSet，表示包含sleep和awake的状态
    return setIndex != SQ_NULL_INDEX && setIndex > SqSetType::sq_disabledSet;
}

const SqTransform& SqJoint::getLocalFrameATransform() const
{
    const SqJointSim *sim = getJointSim();
    return sim->localFrameA;
}

void SqJoint::setLocalFrameATransform(const SqTransform &transform)
{
    SqJointSim *sim = getJointSim();
    sim->localFrameA = transform;
}

const SqTransform& SqJoint::getLocalFrameBTransform() const
{
    SqJointSim *sim = getJointSim();
    return sim->localFrameB;
}

void SqJoint::setConstraintHertz(float hertz)
{
    SqJointSim *sim = getJointSim();
    sim->constraintHertz = hertz;
}

void SqJoint::setConstraintDampingRatio(float damping)
{
    SqJointSim *sim = getJointSim();
    sim->constraintDampingRatio = damping;
}

void SqJoint::setForceThreshold(float force)
{
    SqJointSim *sim = getJointSim();
    sim->forceThreshold = force;
}

void SqJoint::setTorqueThreshold(float torque)
{
    SqJointSim *sim = getJointSim();
    sim->torqueThreshold = torque;
}

void SqJoint::setLocalFrameBTransform(const SqTransform &transform)
{
    SqJointSim *sim = getJointSim();
    sim->localFrameB = transform;
}

void SqJoint::prepare(const SqStepContext &context)
{
    SqJointSim *jointSim = getJointSim();
    float hertz = min(jointSim->constraintHertz, 0.25f * context.inv_h);
    jointSim->constraintSoftness.step(hertz, jointSim->constraintDampingRatio, context.h);
    solvePrepare(context);
}
