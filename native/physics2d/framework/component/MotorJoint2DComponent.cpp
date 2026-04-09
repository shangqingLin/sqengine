#include "MotorJoint2DComponent.h"
#include "../PhysicsSystem.h"

using namespace physics2d;

void MotorJoint2DComponent::onSwitchConnectNode()
{
    tempDef = onCreateJointDef();
    setBody(this->node, connectNode, *tempDef);
    joint = PhysicsSystem::getInstance()->getWorld()->getPyxhWorld()->createMotorJoint(*((phxy::SqMotorJointDef *)tempDef));
    delete tempDef;
    tempDef = nullptr;
}

phxy::SqBaseJointDef *MotorJoint2DComponent::onCreateJointDef()
{
    if (!tempDef)
        tempDef = new phxy::SqMotorJointDef();
    return tempDef;
}

void MotorJoint2DComponent::setMaxForce(float v)
{
    if (joint)
    {
        dynamic_cast<phxy::SqMotorJoint *>(joint)->SetMaxForce(v);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqMotorJointDef *)tempDef)->maxForce = v;
    }
}

void MotorJoint2DComponent::setMaxTorque(float v)
{
    if (joint)
    {
        dynamic_cast<phxy::SqMotorJoint *>(joint)->SetMaxTorque(v);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqMotorJointDef *)tempDef)->maxTorque = v;
    }
}

void MotorJoint2DComponent::setCorrectionFactor(float v)
{
    if (joint)
    {
        dynamic_cast<phxy::SqMotorJoint *>(joint)->SetCorrectionFactor(v);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqMotorJointDef *)tempDef)->correctionFactor = v;
    }
}
