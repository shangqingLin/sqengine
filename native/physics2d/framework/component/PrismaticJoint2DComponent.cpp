#include "PrismaticJoint2DComponent.h"
#include "../PhysicsSystem.h"

using namespace physics2d;

void PrismaticJoint2DComponent::onSwitchConnectNode()
{
    tempDef = onCreateJointDef();
    setBody(this->node, connectNode, *tempDef);
    joint = PhysicsSystem::getInstance()->getWorld()->getPyxhWorld()->createPrimaticJoint(*((phxy::SqPrismaticJointDef *)tempDef));
    delete tempDef;
    tempDef = nullptr;
}

phxy::SqBaseJointDef *PrismaticJoint2DComponent::onCreateJointDef()
{
    if (!tempDef)
        tempDef = new phxy::SqPrismaticJointDef();
    return tempDef;
}

void PrismaticJoint2DComponent::setEnableSpring(bool enable)
{
    if (joint)
    {
        dynamic_cast<phxy::SqPrismaticJoint *>(joint)->EnableSpring(enable);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqPrismaticJointDef *)tempDef)->enableSpring = enable;
    }
}

void PrismaticJoint2DComponent::setHertz(float v)
{
    if (joint)
    {
        dynamic_cast<phxy::SqPrismaticJoint *>(joint)->SetSpringHertz(v);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqPrismaticJointDef *)tempDef)->hertz = v;
    }
}

void PrismaticJoint2DComponent::setDampingRatio(float v)
{
    if (joint)
    {

        dynamic_cast<phxy::SqPrismaticJoint *>(joint)->SetSpringDampingRatio(v);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqPrismaticJointDef *)tempDef)->dampingRatio = v;
    }
}

void PrismaticJoint2DComponent::setEnableLimit(bool enable)
{
    if (joint)
    {

        dynamic_cast<phxy::SqPrismaticJoint *>(joint)->EnableLimit(enable);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqPrismaticJointDef *)tempDef)->enableLimit = enable;
    }
}

void PrismaticJoint2DComponent::setLowerTranslation(float v)
{
    if (joint)
    {

        phxy::SqPrismaticJoint *sJoint = dynamic_cast<phxy::SqPrismaticJoint *>(joint);
        sJoint->SetLimits(v, sJoint->GetUpperLimit());
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqPrismaticJointDef *)tempDef)->lowerTranslation = v;
    }
}

void PrismaticJoint2DComponent::setUpperTranslation(float v)
{
    if (joint)
    {
        phxy::SqPrismaticJoint *sJoint = dynamic_cast<phxy::SqPrismaticJoint *>(joint);
        sJoint->SetLimits(sJoint->GetLowerLimit(), v);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqPrismaticJointDef *)tempDef)->upperTranslation = v;
    }
}

void PrismaticJoint2DComponent::setEnableMotor(bool enable)
{
    if (joint)
    {

        dynamic_cast<phxy::SqPrismaticJoint *>(joint)->EnableMotor(enable);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqPrismaticJointDef *)tempDef)->enableMotor = enable;
    }
}

void PrismaticJoint2DComponent::setMaxMotorForce(float v)
{
    if (joint)
    {

        dynamic_cast<phxy::SqPrismaticJoint *>(joint)->SetMaxMotorForce(v);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqPrismaticJointDef *)tempDef)->maxMotorForce = v;
    }
}

void PrismaticJoint2DComponent::setMotorSpeed(float v)
{
    if (joint)
    {

        dynamic_cast<phxy::SqPrismaticJoint *>(joint)->SetMotorSpeed(v);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqPrismaticJointDef *)tempDef)->motorSpeed = v;
    }
}

void PrismaticJoint2DComponent::SetEnableSyncAngle(bool enableSyncAngle)
{
    if (joint)
    {
        dynamic_cast<phxy::SqPrismaticJoint *>(joint)->SetEnableSyncAngle(enableSyncAngle);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqPrismaticJointDef *)tempDef)->enableSyncAngle = enableSyncAngle;
    }
}