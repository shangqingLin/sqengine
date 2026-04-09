#include "WeldJoint2DComponent.h"
#include "../PhysicsSystem.h"

using namespace physics2d;

void WeldJoint2DComponent::onSwitchConnectNode()
{
    tempDef = onCreateJointDef();
    setBody(this->node, connectNode, *tempDef);
    joint = PhysicsSystem::getInstance()->getWorld()->getPyxhWorld()->createWeldJoint(*((phxy::SqWeldJointDef *)tempDef));
    delete tempDef;
    tempDef = nullptr;
}

phxy::SqBaseJointDef *WeldJoint2DComponent::onCreateJointDef()
{
    if (!tempDef)
        tempDef = new phxy::SqWeldJointDef();
    return tempDef;
}

void WeldJoint2DComponent::setLinearHertz(float v)
{
    if (joint)
    {
        dynamic_cast<phxy::SqWeldJoint *>(joint)->SetLinearHertz(v);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqWeldJointDef *)tempDef)->linearHertz = v;
    }
}
void WeldJoint2DComponent::setAngularHertz(float v)
{
    if (joint)
    {
        dynamic_cast<phxy::SqWeldJoint *>(joint)->SetAngularHertz(v);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqWeldJointDef *)tempDef)->angularHertz = v;
    }
}
void WeldJoint2DComponent::setLinearDampingRatio(float v)
{
    if (joint)
    {
        dynamic_cast<phxy::SqWeldJoint *>(joint)->SetLinearDampingRatio(v);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqWeldJointDef *)tempDef)->linearDampingRatio = v;
    }
}
void WeldJoint2DComponent::setAngularDampingRatio(float v)
{
    if (joint)
    {
        dynamic_cast<phxy::SqWeldJoint *>(joint)->SetAngularDampingRatio(v);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqWeldJointDef *)tempDef)->angularDampingRatio = v;
    }
}
