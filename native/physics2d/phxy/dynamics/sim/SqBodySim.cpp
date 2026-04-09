#include "SqBodySim.h"
#include "../../common/SqConfig.h"

using namespace phxy;

SqBodySim::SqBodySim()
{
    reset();
}

void SqBodySim::reset()
{
    localCenter.zero();
    center.zero();
    transform.identity();
    bodyIndex = SQ_NULL_INDEX;
    flags = 0;

    force.zero();
    invMass = 0.f;
    invInertia = 0.f;

    torque = 0.f;
    linearDamping = 0.f;
    angularDamping = 0.f;
    gravityScale = 1.0f;
    angularVelocity = 0.f;
    linearVelocity.zero();

    deltaPosition.zero();
    deltaRotation.identity();

    maxExtent = 0.f;
    minExtent = SqConfig::getInstance()->getHugValue();

    prevSim.center.zero();
    prevSim.rotation.identity();
}