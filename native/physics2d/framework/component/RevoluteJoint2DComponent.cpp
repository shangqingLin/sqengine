#include "RevoluteJoint2DComponent.h"
#include "../PhysicsSystem.h"
#include "../../../engine/core/math/math.h"

using namespace physics2d;

void RevoluteJoint2DComponent::onSwitchConnectNode()
{
    tempDef = onCreateJointDef();
    setBody(this->node, connectNode, *tempDef);
    joint = PhysicsSystem::getInstance()->getWorld()->getPyxhWorld()->createRevoluteJoint(*((phxy::SqRevoluteJointDef *)tempDef));
    delete tempDef;
    tempDef = nullptr;
}

phxy::SqBaseJointDef *RevoluteJoint2DComponent::onCreateJointDef()
{
    if (!tempDef)
        tempDef = new phxy::SqRevoluteJointDef();
    return tempDef;
}

void RevoluteJoint2DComponent::setTargetAngle(float angle)
{
    angle = Math::angleToRadian(angle);
    if (joint)
    {
        dynamic_cast<phxy::SqRevoluteJoint *>(joint)->setTargetAngle(angle);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqRevoluteJointDef *)tempDef)->targetAngle = angle;
    }
}

void RevoluteJoint2DComponent::setEnableSpring(bool enable)
{
    if (joint)
    {
        dynamic_cast<phxy::SqRevoluteJoint *>(joint)->setEnableSpring(enable);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqRevoluteJointDef *)tempDef)->enableSpring = enable;
    }
}

void RevoluteJoint2DComponent::setHertz(float v)
{
    if (joint)
    {
        dynamic_cast<phxy::SqRevoluteJoint *>(joint)->setSpringHertz(v);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqRevoluteJointDef *)tempDef)->hertz = v;
    }
}

void RevoluteJoint2DComponent::setDampingRatio(float v)
{
    if (joint)
    {
        dynamic_cast<phxy::SqRevoluteJoint *>(joint)->setSpringDampingRatio(v);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqRevoluteJointDef *)tempDef)->dampingRatio = v;
    }
}

void RevoluteJoint2DComponent::setEnableLimit(bool enable)
{
    if (joint)
    {
        dynamic_cast<phxy::SqRevoluteJoint *>(joint)->setEnableLimit(enable);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqRevoluteJointDef *)tempDef)->enableLimit = enable;
    }
}

void RevoluteJoint2DComponent::setLowerAngle(float angle)
{
    angle = Math::angleToRadian(angle);
    if (joint)
    {
        phxy::SqRevoluteJoint *reJoint = static_cast<phxy::SqRevoluteJoint *>(joint);
        reJoint->setLimits(angle, reJoint->getUpperLimit());
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqRevoluteJointDef *)tempDef)->lowerAngle = angle;
    }
}

void RevoluteJoint2DComponent::setUpperAngle(float angle)
{
    angle = Math::angleToRadian(angle);
    if (joint)
    {
        phxy::SqRevoluteJoint *reJoint = static_cast<phxy::SqRevoluteJoint *>(joint);
        reJoint->setLimits(reJoint->getLowerLimit(), angle);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqRevoluteJointDef *)tempDef)->upperAngle = angle;
    }
}

void RevoluteJoint2DComponent::setEnableMotor(bool enable)
{
    if (joint)
    {
        dynamic_cast<phxy::SqRevoluteJoint *>(joint)->setEnableMotor(enable);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqRevoluteJointDef *)tempDef)->enableMotor = enable;
    }
}

void RevoluteJoint2DComponent::setMaxMotorTorque(float v)
{
    if (joint)
    {
        dynamic_cast<phxy::SqRevoluteJoint *>(joint)->setMaxMotorTorque(v);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqRevoluteJointDef *)tempDef)->maxMotorTorque = v;
    }
}

void RevoluteJoint2DComponent::setMotorSpeed(float v)
{
    v = Math::angleToRadian(v);
    if (joint)
    {
        dynamic_cast<phxy::SqRevoluteJoint *>(joint)->setMotorSpeed(v);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqRevoluteJointDef *)tempDef)->motorSpeed = v;
    }
}
