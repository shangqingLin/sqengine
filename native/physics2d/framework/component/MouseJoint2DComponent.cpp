#include "MouseJoint2DComponent.h"
#include "../PhysicsSystem.h"

using namespace physics2d;

MouseJoint2DComponent::MouseJoint2DComponent()
{
}

void MouseJoint2DComponent::onSwitchConnectNode()
{
    tempDef = onCreateJointDef();
    setBody(this->node, connectNode, *tempDef);
    joint = PhysicsSystem::getInstance()->getWorld()->getPyxhWorld()->createMouseJoint(*((phxy::SqMouseJointDef *)tempDef));
    delete tempDef;
    tempDef = nullptr;
}

phxy::SqBaseJointDef *MouseJoint2DComponent::onCreateJointDef()
{
    if (!tempDef)
        tempDef = new phxy::SqMouseJointDef();
    return tempDef;
}

void MouseJoint2DComponent::setTarget(Vec2 &vec)
{
    // if (joint)
    // {
    //     b2Vec2 v;
    //     v.x = vec.x;
    //     v.y = vec.y;
    //     // b2MouseJoint_SetTarget(dynamic_cast<phxy::SqMouseJoint *>(joint)->joinId, v);
    // }
    // else
    // {
    //     tempDef = onCreateJointDef();
    //     Vec2 v;
    //     v.x = vec.x;
    //     v.y = vec.y;
    //     ((phxy::SqMouseJointDef *)tempDef)->target = v;
    // }
}

void MouseJoint2DComponent::setHertz(float v)
{
    if (joint)
    {
        dynamic_cast<phxy::SqMouseJoint *>(joint)->SetSpringHertz(v);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqMouseJointDef *)tempDef)->hertz = v;
    }
}

void MouseJoint2DComponent::setDampingRatio(float v)
{
    if (joint)
    {
        dynamic_cast<phxy::SqMouseJoint *>(joint)->SetSpringDampingRatio(v);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqMouseJointDef *)tempDef)->dampingRatio = v;
    }
}

void MouseJoint2DComponent::setMaxForce(float v)
{
    if (joint)
    {
        dynamic_cast<phxy::SqMouseJoint *>(joint)->SetMaxForce(v);
    }
    else
    {
        tempDef = onCreateJointDef();
        ((phxy::SqMouseJointDef *)tempDef)->maxForce = v;
    }
}
