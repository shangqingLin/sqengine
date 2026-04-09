#include "WheelJoint2DComponent.h"
#include "../PhysicsSystem.h"
#include "../../../engine/core/core.h"

using namespace physics2d;

void WheelJoint2DComponent::onSwitchConnectNode()
{
    tempDef = onCreateJointDef();
    setBody(this->node, connectNode, *tempDef);
    joint = PhysicsSystem::getInstance()->getWorld()->getPyxhWorld()->createWheelJoint(*((phxy::SqWheelJointDef *)tempDef));
    delete tempDef;
    tempDef = nullptr;
}

phxy::SqBaseJointDef *WheelJoint2DComponent::onCreateJointDef()
{
    if (!tempDef)
        tempDef = new phxy::SqWheelJointDef();
    return tempDef;
}

void WheelJoint2DComponent::setEnableSpring(float enable)
{
    if (joint)
    {

        dynamic_cast<phxy::SqWheelJoint *>(joint)->EnableSpring(enable);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqWheelJointDef *)tempDef)->enableSpring = enable;
    }
}

void WheelJoint2DComponent::setHertz(float v)
{
    if (joint)
    {
        dynamic_cast<phxy::SqWheelJoint *>(joint)->SetSpringHertz(v);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqWheelJointDef *)tempDef)->hertz = v;
    }
}

void WheelJoint2DComponent::setDampingRatio(float v)
{
    if (joint)
    {
        dynamic_cast<phxy::SqWheelJoint *>(joint)->SetSpringDampingRatio(v);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqWheelJointDef *)tempDef)->dampingRatio = v;
    }
}

void WheelJoint2DComponent::setEnableLimit(bool enable)
{
    if (joint)
    {
        dynamic_cast<phxy::SqWheelJoint *>(joint)->EnableLimit(enable);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqWheelJointDef *)tempDef)->enableLimit = enable;
    }
}

void WheelJoint2DComponent::setLowerTranslation(float t)
{
    if (joint)
    {
        phxy::SqWheelJoint *wjoint = dynamic_cast<phxy::SqWheelJoint *>(joint);
        wjoint->SetLimits(t, wjoint->GetUpperLimit());
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqWheelJointDef *)tempDef)->lowerTranslation = t;
    }
}

void WheelJoint2DComponent::setUpperTranslation(float t)
{
    if (joint)
    {
        phxy::SqWheelJoint *wjoint = dynamic_cast<phxy::SqWheelJoint *>(joint);
        wjoint->SetLimits(wjoint->GetLowerLimit(), t);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqWheelJointDef *)tempDef)->upperTranslation = t;
    }
}

void WheelJoint2DComponent::setEnableMotor(bool enable)
{
    if (joint)
    {
        dynamic_cast<phxy::SqWheelJoint *>(joint)->EnableMotor(enable);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqWheelJointDef *)tempDef)->enableMotor = enable;
    }
}

void WheelJoint2DComponent::setMaxMotorTorque(float v)
{
    if (joint)
    {
        dynamic_cast<phxy::SqWheelJoint *>(joint)->SetMaxMotorTorque(v);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqWheelJointDef *)tempDef)->maxMotorTorque = v;
    }
}

void WheelJoint2DComponent::setMotorSpeed(float v)
{
    v = Math::angleToRadian(v);
    if (joint)
    {
        dynamic_cast<phxy::SqWheelJoint *>(joint)->SetMotorSpeed(v);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqWheelJointDef *)tempDef)->motorSpeed = v;
    }
}