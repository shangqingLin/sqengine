#include "DistanceJoint2DComponent.h"
#include "../PhysicsSystem.h"

using namespace physics2d;

void DistanceJoint2DComponent::onSwitchConnectNode()
{
    tempDef = onCreateJointDef();
    setBody(this->node, connectNode, *tempDef);
    joint = PhysicsSystem::getInstance()->getWorld()->getPyxhWorld()->createDistanceJoint(*((phxy::SqDistanceJointDef *)tempDef));
    delete tempDef;
    tempDef = nullptr;
}

phxy::SqBaseJointDef *DistanceJoint2DComponent::onCreateJointDef()
{
    if (!tempDef)
        tempDef = new phxy::SqDistanceJointDef();
    return tempDef;
}

void DistanceJoint2DComponent::setLength(float v)
{
    if (joint)
    {
        dynamic_cast<phxy::SqDistanceJoint *>(joint)->SetLength(v);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqDistanceJointDef *)tempDef)->length = v;
    }
}

void DistanceJoint2DComponent::setEnableSpring(bool enable)
{
    if (joint)
    {
        dynamic_cast<phxy::SqDistanceJoint *>(joint)->EnableSpring(enable);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqDistanceJointDef *)tempDef)->enableSpring = enable;
    }
}

void DistanceJoint2DComponent::setHertz(float v)
{
    if (joint)
    {
        dynamic_cast<phxy::SqDistanceJoint *>(joint)->SetSpringHertz(v);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqDistanceJointDef *)tempDef)->hertz = v;
    }
}

void DistanceJoint2DComponent::setDampingRatio(float v)
{
    if (joint)
    {
        dynamic_cast<phxy::SqDistanceJoint *>(joint)->SetSpringDampingRatio(v);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqDistanceJointDef *)tempDef)->dampingRatio = v;
    }
}

void DistanceJoint2DComponent::setEnableLimit(bool enable)
{
    if (joint)
    {
        dynamic_cast<phxy::SqDistanceJoint *>(joint)->EnableLimit(enable);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqDistanceJointDef *)tempDef)->enableLimit = enable;
    }
}

void DistanceJoint2DComponent::setMaxLength(float v)
{
    if (joint)
    {
        phxy::SqDistanceJoint *dJoint = dynamic_cast<phxy::SqDistanceJoint *>(joint);
        dJoint->SetLengthRange(dJoint->GetMinLength(), v);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqDistanceJointDef *)tempDef)->maxLength = v;
    }
}

void DistanceJoint2DComponent::setMaxMotorForce(float v)
{
    if (joint)
    {
        dynamic_cast<phxy::SqDistanceJoint *>(joint)->SetMaxMotorForce(v);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqDistanceJointDef *)tempDef)->maxMotorForce = v;
    }
}

void DistanceJoint2DComponent::setEnableMotor(bool enable)
{
    if (joint)
    {
        dynamic_cast<phxy::SqDistanceJoint *>(joint)->EnableMotor(enable);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqDistanceJointDef *)tempDef)->enableMotor = enable;
    }
}

void DistanceJoint2DComponent::setMinLength(float v)
{
    if (joint)
    {
        phxy::SqDistanceJoint *dJoint = dynamic_cast<phxy::SqDistanceJoint *>(joint);
        dJoint->SetLengthRange(v, dJoint->GetMaxLength());
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqDistanceJointDef *)tempDef)->minLength = v;
    }
}

void DistanceJoint2DComponent::setMotorSpeed(float v)
{
    if (joint)
    {
        dynamic_cast<phxy::SqDistanceJoint *>(joint)->SetMotorSpeed(v);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqDistanceJointDef *)tempDef)->motorSpeed = v;
    }
}
